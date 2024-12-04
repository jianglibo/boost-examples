#include <gtest/gtest.h>  // Add this line

#include <boost/json.hpp>
#include <boost/process.hpp>
#include <boost/url.hpp>
#include <cstddef>  // For std::ptrdiff_t
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>  // For std::numeric_limits

#include "common_util.hpp"
#include "ssh_pk_process.hpp"
#include "ssh_process.hpp"

// https://www.boost.org/doc/libs/1_86_0/doc/html/boost_process/v2.html#boost_process.v2.introduction.pidfd_open

TEST(BoostExampleProcessTest, starter) {
  common::ThreadNotifier notifier(1000);
  auto& iocManager = common::get_db_io_context_manager();
  bpv2::process proc(iocManager.ioc(), "/usr/bin/ls", {},
                     bpv2::process_stdio{{/*in to default*/}, stdout, stdout});

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
  bpv2::process proc(iocManager.ioc(), "/usr/bin/sleep", {"10"});
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
  bpv2::process proc(iocManager.ioc(), "/usr/bin/sleep", {"10"});
  std::cout << "pos 1" << std::endl;
  proc.wait();
  std::cout << "pos 2" << std::endl;
  notifier.waitForNotification();
  iocManager.stop();
}

TEST(BoostExampleProcessTest, toFile) {
  UniqueFile out(fopen("output.txt", "w"));
  common::ThreadNotifier notifier(1000);
  auto& iocManager = common::get_db_io_context_manager();
  bpv2::process proc(iocManager.ioc(), "/usr/bin/ls", {},
                     bpv2::process_stdio{{/*in to default*/}, out.get(), {}});
  proc.wait();
  std::ifstream in("output.txt");
  std::string content((std::istreambuf_iterator<char>(in)),
                      std::istreambuf_iterator<char>());
  std::cout << "out content: \n" << content << std::endl;
}

TEST(BoostExampleProcessTest, sshtunnel) {
  common::ThreadNotifier notifier(30000);

  bpv2::environment::value v = bpv2::environment::get("PATH");
  // std::cout << "path: " << v << std::endl;
  std::unordered_map<bpv2::environment::key, bpv2::environment::value> my_env =
      {{"SECRET", "THIS_IS_A_TEST"}};

  // my_env["PATH"] = bpv2::environment::value("/bin", "/usr/bin");

  std::cout << "pos 0" << std::endl;
  // auto sshProc =
  //     std::make_shared<SshProcess>(common::get_db_io_context_manager().ioc(),
  //                                  bpv2::process_environment(my_env));
  std::make_shared<SshProcess>(common::get_db_io_context_manager().ioc(),
                               bpv2::environment::current_view(), 1000ms)
      ->start();
  std::cout << "pos 1" << std::endl;
  notifier.waitForNotification();
}

TEST(BoostExampleProcessTest, sshpk) {
  common::ThreadNotifier notifier(6000);
  bpv2::environment::value v = bpv2::environment::get("PATH");
  std::unordered_map<bpv2::environment::key, bpv2::environment::value> my_env =
      {{"SECRET", "THIS_IS_A_TEST"}};

  std::make_shared<SshPkProcess>(common::get_db_io_context_manager().ioc(),
                                 bpv2::environment::current_view())
      ->start();
  std::cout << "pos 1" << std::endl;
  notifier.waitForNotification();
}