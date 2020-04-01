#pragma once
#include <atomic>
#include <functional>
#include <iris/operation.hpp>
#include <iris/task_system.hpp>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <zmq.hpp>

namespace iris {

class zmq_subscriber {
  std::reference_wrapper<zmq::context_t> context_;
  std::reference_wrapper<task_system> executor_;
  std::unique_ptr<zmq::socket_t> socket_;
  std::vector<std::string> endpoints_;
  std::string filter_;
  operation::string_argument fn_;
  std::thread thread_;
  std::atomic<bool> done_{false};

public:
  zmq_subscriber(zmq::context_t &context, std::vector<std::string> endpoints,
                 std::string filter, const operation::string_argument &fn,
                 task_system &executor)
      : context_(context), endpoints_(std::move(endpoints)),
        filter_(std::move(filter)), fn_(fn), executor_(executor) {
    socket_ = std::make_unique<zmq::socket_t>(context, ZMQ_SUB);
    for (auto &e : endpoints_) {
      socket_->connect(e);
    }
    socket_->setsockopt(ZMQ_SUBSCRIBE, filter_.c_str(), filter_.length());
    socket_->setsockopt(ZMQ_RCVTIMEO, 0);
  }

  ~zmq_subscriber() {
    thread_.join();
    socket_->close();
  }

  void recv() {
    while (!done_) {
      zmq::message_t received_message;
      socket_->recv(&received_message);
      const auto message =
          std::string(static_cast<char *>(received_message.data()),
                      received_message.size());
      if (message.length() > 0) {
        fn_.arg = std::move(message);
        executor_.get().async_(fn_);
      }
    }
  }

  void start() { thread_ = std::thread(&zmq_subscriber::recv, this); }

  void stop() { done_ = true; }
};

} // namespace iris