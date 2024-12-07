#pragma once

#include <boost/asio.hpp>
#include <cstdio>
#include <iostream>
#include <thread>

namespace net = boost::asio;  // from <boost/asio.hpp>
namespace bpv2 = boost::process::v2;

struct FileDeleter {
  void operator()(FILE* file) const {
    if (file) fclose(file);
  }
};

using UniqueFile = std::unique_ptr<FILE, FileDeleter>;

namespace common {

class ThreadNotifier {
 public:
  ThreadNotifier(uint64_t milliseconds = 0ull)
      : notified_(false), milliseconds_(milliseconds) {}

  void waitForNotification() {
    std::unique_lock<std::mutex> lock(mutex_);
    // Wait until notified
    if (milliseconds_ == 0) {
      // In other words:
      // It wakes up when the condition variable is notified.
      // It continues execution only if the predicate evaluates to true.
      // If the predicate returns false, it will keep waiting.
      condition_.wait(
          lock, [this] { return notified_; });  // predicate checks if notified
    } else {
      // Single Timeout Period: wait_for only waits for the specified
      // duration once, measured from the moment the function is called.
      // Notification Handling: If the condition variable is notified during the
      // timeout: The predicate is evaluated. If the predicate returns true, the
      // thread proceeds. If the predicate returns false, the thread continues
      // waiting within the same timeout period. Timeout Expiry: If the
      // specified duration expires, wait_for returns, regardless of the
      // predicate's state.
      condition_.wait_for(lock, std::chrono::milliseconds(milliseconds_),
                          [this] { return notified_; });
    }
    notified_ = false;  // Reset the notification status
    std::cout << "Thread notified!" << std::endl;
  }

  void notify() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      notified_ = true;  // Set the notification status
    }
    condition_.notify_one();  // Notify one waiting thread
  }

 private:
  std::mutex mutex_;
  std::condition_variable condition_;
  bool notified_;
  uint64_t milliseconds_;
};

class IoContextManager {
 public:
  IoContextManager(int threads_num = 0)
#ifdef DEBUG_BUILD
      : threads_num_(1),
#else
      : threads_num_(threads_num == 0 ? std::thread::hardware_concurrency()
                                      : threads_num),
#endif
        ioc_(threads_num_),
        work_guard(net::make_work_guard(ioc_)) {
    threads_.reserve(threads_num_);
    for (int i = 0; i < threads_num_; ++i) {
      threads_.emplace_back([this] { ioc_.run(); });
    }
  }

  boost::asio::io_context& ioc() { return ioc_; }

  void stop() {
    ioc_.stop();
    for (auto& thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  ~IoContextManager() { stop(); }

 private:
  int threads_num_;
  std::vector<std::thread> threads_;
  boost::asio::io_context ioc_;
  net::executor_work_guard<net::io_context::executor_type> work_guard;
};

inline IoContextManager& get_net_io_context_manager(int threads_num = 0) {
  static IoContextManager manager(threads_num);
  return manager;
}

inline IoContextManager& get_db_io_context_manager(int threads_num = 0) {
  static IoContextManager manager(threads_num);
  return manager;
}


}  // namespace common