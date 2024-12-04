#include <gtest/gtest.h>  // Add this line

#include <boost/json.hpp>
#include <boost/process.hpp>
#include <boost/url.hpp>
#include <cstddef>  // For std::ptrdiff_t
#include <iostream>
#include <limits>  // For std::numeric_limits

#include "common_util.hpp"

namespace pv2 = boost::process::v2;

// https://www.boost.org/doc/libs/1_86_0/doc/html/boost_process/v2.html#boost_process.v2.introduction.pidfd_open

TEST(BoostExampleProcessTest, starter) {
  common::ThreadNotifier notifier(1000);
  auto& iocManager = common::get_db_io_context_manager();
  pv2::process proc(iocManager.ioc(), "/usr/bin/ls", {},
                    pv2::process_stdio{{/*in to default*/}, stdout, stdout});

  std::cout << "pos 1" << std::endl;
  proc.async_wait(
      [&notifier](const boost::system::error_code& ec, int exit_code) {
        if (ec) {
          std::cerr << "Error: " << ec.message() << std::endl;
        } else {
          std::cout << "Exit code: " << exit_code << std::endl;
        }
        notifier.notify();
      });
  std::cout << "pos 2" << std::endl;
  notifier.waitForNotification();
  iocManager.stop();
}

TEST(BoostExampleProcessTest, sleep) {
  common::ThreadNotifier notifier(1000);
  auto& iocManager = common::get_db_io_context_manager();
  pv2::process proc(iocManager.ioc(), "/usr/bin/sleep", {"10"});
  std::cout << "pos 1" << std::endl;
  proc.async_wait(
      [&notifier](const boost::system::error_code& ec, int exit_code) {
        if (ec) {
          std::cerr << "Error: " << ec.message() << std::endl;
        } else {
          std::cout << "Exit code: " << exit_code << std::endl;
        }
        notifier.notify();
      });
  std::cout << "pos 2" << std::endl;
  notifier.waitForNotification();
  iocManager.stop();
}

TEST(BoostExampleProcessTest, sleepsync) {
  common::ThreadNotifier notifier(1000);
  auto& iocManager = common::get_db_io_context_manager();
  pv2::process proc(iocManager.ioc(), "/usr/bin/sleep", {"10"});
  std::cout << "pos 1" << std::endl;
  proc.wait();
  std::cout << "pos 2" << std::endl;
  notifier.waitForNotification();
  iocManager.stop();
}
TEST(BoostExampleProcessTest, rpipe) {
  auto& iocManager = common::get_db_io_context_manager();
  common::ThreadNotifier notifier(1000);
  net::readable_pipe rp{iocManager.ioc()};
  pv2::process proc(
      iocManager.ioc(), "/usr/bin/g++", {"--version"},
      pv2::process_stdio{{/* in to default */}, rp, {/* err to default */}});
  boost::system::error_code ec;
  std::string output;
  std::string some_output;
  while (size_t byte = rp.read_some(net::buffer(some_output), ec)) {
    if (ec == net::error::eof) {
      break;
    }
    output += some_output;
  }
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  proc.wait();
  std::cout << "Output: " << output << std::endl;
  notifier.waitForNotification();
}