#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/process.hpp>
#include <chrono>
#include <fstream>
#include <map>
#include <vector>

#include "common_util.hpp"

using namespace std::chrono_literals;

class SshPkProcess : public std::enable_shared_from_this<SshPkProcess> {
 public:
  SshPkProcess(net::io_context& ioc, bpv2::process_environment&& env,
               std::chrono::milliseconds interval = 0ms)
      : ioc_(ioc), env_(std::move(env)), interval_(interval) {
    if (interval_ > 0ms) {
      interval_timer_ = net::steady_timer(ioc_, interval);
    }
    // net::dynamic_string_buffer buf(output_);
  }

  // bpv2::process p(ioc_, bpv2::environment::find_executable("ssh"),
  //                 {"jianglibo@192.168.0.21", "cat t.txt"},
  //                 bpv2::process_stdio{wp_.value(), rp_.value(), {}}, env_);

  void start() {
    bpv2::process agent(ioc_, bpv2::environment::find_executable("ssh-agent"),
                        {"-s"}, bpv2::process_stdio{}, env_);
    agent.async_wait([self = this->shared_from_this()](
                         const boost::system::error_code& ec, int exit_code) {
      if (ec) {
        std::cerr << "Error: " << ec.message() << std::endl;
      } else {
        std::cout << "Exit code: " << exit_code << std::endl;
        self->after_agent();
      }
    });
    agent_.emplace(std::move(agent));
  }

  void after_agent() {
    rp_.emplace(ioc_);
    wp_.emplace(ioc_);
    bpv2::process p(ioc_, bpv2::environment::find_executable("ssh"),
                    {"-i", "/dev/stdin", "jianglibo@192.168.0.21", "cat t.txt"},
                    bpv2::process_stdio{wp_.value(), rp_.value(), {}}, env_);
    proc_.emplace(std::move(p));
    std::ifstream ifs("/home/jianglibo/.ssh/id_rsa");
    if (!ifs) {
      std::cerr << "Cannot open file." << std::endl;
      return;
    }
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    std::cout << "content: " << content << std::endl;
    size_t wb = wp_->write_some(net::buffer(content));
    std::cout << "wb: " << wb << " of content length: " << content.size()
              << std::endl;
    wp_->close();
    handle_proc();
    if (interval_ > 0ms) {
      run_interval();
    }
  }

 private:
  net::io_context& ioc_;
  bpv2::process_environment env_;
  std::optional<net::readable_pipe> rp_;
  std::optional<net::writable_pipe> wp_;
  std::optional<bpv2::process> proc_;
  std::optional<bpv2::process> agent_;
  std::array<char, 1024> some_output_;
  std::string output_;
  std::chrono::milliseconds interval_;
  std::optional<net::steady_timer> interval_timer_;
  uint64_t interval_counter_;

  void handle_proc() {
    if (!rp_->is_open()) {
      return;
    }
    bpv2::environment::set("a", "b");
    std::cout << std::boolalpha << "rp is open: " << rp_->is_open()
              << std::endl;
    // rp_.async_read_some(net::buffer(some_output_),
    rp_->async_read_some(
        net::buffer(some_output_),
        [self = this->shared_from_this()](const boost::system::error_code& ec,
                                          size_t byte) {
          std::cout << "bytes read: " << byte << std::endl;
          if (ec) {
            if (ec == net::error::eof) {
              std::cout << "EOF reached." << std::endl;
              std::cout << "output: " << self->output_ << std::endl;
              return;
            }
            std::cerr << "Error: " << ec.message() << std::endl;
          } else {
            self->handle_proc();
            self->output_ += std::string(self->some_output_.data(), byte);
          }
        });
  }

  void run_interval() {
    interval_timer_->expires_after(interval_);
    interval_timer_->async_wait(
        [self = this->shared_from_this()](const boost::system::error_code& ec) {
          self->interval_counter_++;
          if (ec) {
            std::cerr << "Error: " << ec.message() << std::endl;
          } else {
            self->run_interval();
            std::cout << std::boolalpha << "Running? " << self->proc_->running()
                      << std::endl;
          }
        });
  }
};