#ifndef CHROME_TRACER_TRACER_H_
#define CHROME_TRACER_TRACER_H_

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <mutex>

#include "event.h"

namespace chrome_tracer {

class ChromeTracer {
 public:
  ChromeTracer() = default;
  ChromeTracer(std::string name);

  bool HasStream(std::string stream);
  void AddStream(std::string stream);

  bool HasEvent(std::string stream, int32_t handle);
  void MarkEvent(std::string stream, std::string event_name, std::string metadata);
  int32_t BeginEvent(std::string stream, std::string event_name, std::string metadata);
  void EndEvent(std::string stream, int32_t handle);

  bool Validate() const;

  std::string Dump(bool force = false) const;
  void Dump(std::string path, bool force = false) const;
  std::string Summary() const;

  void Clear() {
    event_table_.clear();
    anchor_ = std::chrono::system_clock::now();
  }

 protected:
  std::string name_;
  std::map<std::string, std::map<int32_t, Event>> event_table_;
  std::chrono::system_clock::time_point anchor_;
  
  size_t count_;

  mutable std::mutex lock_;
};

}  // namespace chrome_tracer

#endif  // CHROME_TRACER_TRACER_H_