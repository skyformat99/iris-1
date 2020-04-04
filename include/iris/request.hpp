#pragma once
#include <iris/cppzmq/zmq.hpp>

namespace iris {

namespace internal {
class ServerImpl;
}

class Request {
  zmq::message_t payload_;
  class Component *component_;
  std::uint8_t server_id_;
  friend class TaskSystem;
  friend class internal::ServerImpl;

public:
  Request() {}

  Request(const Request &rhs) {
    payload_.copy(const_cast<Request &>(rhs).payload_);
    component_ = rhs.component_;
    server_id_ = rhs.server_id_;
  }

  Request &operator=(Request rhs) {
    std::swap(payload_, rhs.payload_);
    std::swap(component_, rhs.component_);
    std::swap(server_id_, rhs.server_id_);
    return *this;
  }

  template <typename T> T get();
};

}; // namespace iris