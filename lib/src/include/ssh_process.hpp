#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/process.hpp>
#include <chrono>
#include <map>
#include <vector>

#include "common_util.hpp"

using namespace std::chrono_literals;

class SshProcess : public std::enable_shared_from_this<SshProcess> {
 public:
  SshProcess(net::io_context& ioc, bpv2::process_environment&& env,
             std::chrono::milliseconds interval = 0ms)
      : ioc_(ioc), env_(std::move(env)), interval_(interval) {
    if (interval_ > 0ms) {
      interval_timer_ = net::steady_timer(ioc_, interval);
    }
    // net::dynamic_string_buffer buf(output_);
  }

  void start() {
    rp_.emplace(ioc_);
    wp_.emplace(ioc_);
    bpv2::process p(ioc_, bpv2::environment::find_executable("ssh"),
                    {"-D", "8090", "-C", "-N", "jianglibo@192.168.0.21"},
                    bpv2::process_stdio{{}, rp_.value(), {}}, env_);
    proc_.emplace(std::move(p));
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
              return;
            }
            std::cerr << "Error: " << ec.message() << std::endl;
          } else {
            self->handle_proc();
            std::cout << "Output: "
                      << std::string(self->some_output_.data(), byte)
                      << std::endl;
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