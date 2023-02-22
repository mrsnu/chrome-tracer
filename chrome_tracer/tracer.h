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

  bool HasEvent(std::string stream, std::string event);
  std::string BeginEvent(std::string stream, std::string event, bool exist_ok = false);
  void EndEvent(std::string stream, std::string event, bool exist_ok = false);

  bool Validate() const;

  std::string Dump() const;
  void Dump(std::string path) const;
  std::string Summary() const;

  void Clear();

 private:
  std::string name_;
  std::map<std::string, std::map<std::string, Event>> event_table_;
  std::chrono::system_clock::time_point anchor_;
  
  size_t count_;

  mutable std::mutex lock_;
};

}  // namespace chrome_tracer

#endif  // CHROME_TRACER_TRACER_H_