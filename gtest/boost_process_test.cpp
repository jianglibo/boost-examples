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

TEST(BoostExampleProcessTest, teminatetask) {
  common::ThreadNotifier notifier(100000);
  auto proc =
      bpv2::process(common::get_db_io_context_manager().ioc(),
                    bpv2::environment::find_executable("sleep"), {"10"});
  proc.async_wait([&](boost::system::error_code ec, int exit_code) {
    if (ec) {
      std::cerr << "Error........: " << ec.message() << std::endl;
    } else {
      std::cout << "Exit code........: " << exit_code << std::endl;
    }
    // sig.emit(net::cancellation_type::partial);
  });
  std::cout << "proc id: " << proc.id() << std::endl;

  net::steady_timer timeout{common::get_db_io_context_manager().ioc(), 2s};

  timeout.async_wait([&](const boost::system::error_code& ec) {
    if (ec) {
      std::cerr << "Error: " << ec.message() << std::endl;
    } else {
      std::cout << "Timeout!" << std::endl;
      proc.terminate();
      std::cout << "Timeout!--" << std::endl;

      std::cout << "exit code: " << proc.exit_code() << std::endl;
      std::cout << std::boolalpha << "is open: " << proc.is_open() << std::endl;
      boost::system::error_code ec;
      std::cout << std::boolalpha << "is running: " << proc.running(ec)
                << std::endl;
      if (ec) {
        std::cerr << "query running Error: " << ec.message() << std::endl;
      }
      notifier.notify();
    }
  });

  notifier.waitForNotification();
}

TEST(BoostExampleProcessTest, canceltask) {
  common::ThreadNotifier notifier(11000);
  net::cancellation_signal sig;
  // auto proc =
  //     bpv2::process(common::get_db_io_context_manager().ioc(),
  //                   bpv2::environment::find_executable("sleep"), {"10"});
  // proc.async_wait([&](boost::system::error_code ec, int exit_code) {
  //   if (ec) {
  //     std::cerr << "Error: " << ec.message() << std::endl;
  //   } else {
  //     std::cout << "Exit code: " << exit_code << std::endl;
  //   }
  //   // sig.emit(net::cancellation_type::partial);
  // });

  // proc.terminate();
  bpv2::async_execute(bpv2::process(common::get_db_io_context_manager().ioc(),
                                    "/usr/bin/g++", {"--version"}),
                      [&](boost::system::error_code ec, int exit_code) {
                        if (ec) {
                          std::cerr << "Error: " << ec.message() << std::endl;
                        } else {
                          std::cout << "Exit code: " << exit_code << std::endl;
                        }
                        // sig.emit(net::cancellation_type::partial);
                        sig.emit(net::cancellation_type::terminal);
                      });

  bpv2::async_execute(
      bpv2::process(common::get_db_io_context_manager().ioc(), "/usr/bin/sleep",
                    {"10"}),
      net::bind_cancellation_slot(
          sig.slot(), [&](boost::system::error_code ec, int exit_code) {
            // timeout.cancel();  // we're done earlier
            if (ec) {
              std::cerr << "Error1: " << ec.message() << std::endl;
            } else {
              std::cout << "Exit code1: " << exit_code << std::endl;
            }
            notifier.notify();
          }));

  bpv2::execute(bpv2::process(common::get_db_io_context_manager().ioc(),
                              bpv2::environment::find_executable("echo"),
                              {"'*******version'"}));

  notifier.waitForNotification();
}