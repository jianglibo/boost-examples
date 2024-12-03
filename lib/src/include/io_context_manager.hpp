#pragma once

#include <boost/asio.hpp>
#include <thread>

namespace net = boost::asio;  // from <boost/asio.hpp>

namespace common {
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

  boost::asio::io_context& get_io_context() { return ioc_; }

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