#ifndef CHROME_TRACER_EVENT_H_
#define CHROME_TRACER_EVENT_H_

#include <iostream>
#include <string>
#include <chrono>

namespace chrome_tracer {

struct Event {
  enum class EventStatus {
    Running = 0,
    Finished = 1,
    Instantanous = 2,
  };

  Event() = delete;

  Event(std::string name,
        std::string metadata,
        std::chrono::system_clock::time_point start,
        EventStatus status = EventStatus::Running) : 
      name(name),
      metadata(metadata),
      status_(status),
      start(start) {}

  void Finish() {
    if (status_ != EventStatus::Running) {
      std::cerr << "Cannot finish the event not running";
      abort();
    }
    end = std::chrono::system_clock::now();
    status_ = EventStatus::Finished;
  }

  EventStatus GetStatus() const {
    return status_;
  }

  std::chrono::system_clock::time_point start;
  std::chrono::system_clock::time_point end;
  std::string name;
  std::string metadata;

 private:
  EventStatus status_;
};

}  // namepsace chrome_tracer

#endif  // CHROME_TRACER_EVENT_H_