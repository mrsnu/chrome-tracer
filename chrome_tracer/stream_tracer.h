#ifndef CHROME_TRACER_STREAM_TRACER_H_
#define CHROME_TRACER_STREAM_TRACER_H_

#include <mutex>
#include <fstream>
#include <set>
#include <map>

#include "chrome_tracer/tracer_interface.h"

namespace chrome_tracer {

class ChromeStreamTracer : public ITracer {
 public:
  ChromeStreamTracer(std::string path);
  ChromeStreamTracer(std::string name, std::string path);
  ~ChromeStreamTracer();

  bool HasStream(std::string stream) const;
  void AddStream(std::string stream);

  bool HasEvent(std::string stream, int32_t handle);
  void MarkEvent(std::string stream, std::string event);
  int32_t BeginEvent(std::string stream, std::string event);
  void EndEvent(std::string stream, int32_t handle, std::string args = "");

  bool Validate() const;

  std::string Dump() const;
  void Dump(std::string path) const;
  std::string Summary() const;

  void Clear();

 private:
  std::string name_;
  std::string path_;
  std::ofstream file_;

  std::chrono::system_clock::time_point anchor_;
  
  std::map<std::string, int32_t> stream_table_;
  std::map<int32_t, Event> event_table_;

  int32_t count_;
  const size_t pid_ = 0;
  
  mutable std::mutex lock_;
};

}  // namespace chrome_tracer

#endif  // CHROME_TRACER_STREAM_TRACER_H_