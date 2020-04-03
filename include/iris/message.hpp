#pragma once
#include <string>

namespace iris {

class Message {
  std::string payload_;
  class Component *component_;
  std::uint8_t subscriber_id_;
  friend class SubscriberImpl;

public:
  template <typename T> T deserialize();
};

template <> inline std::string Message::deserialize() { return payload_; }

}; // namespace iris