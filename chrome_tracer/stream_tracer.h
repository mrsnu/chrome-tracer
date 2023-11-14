#ifndef CHROME_TRACER_STREAM_TRACER_H_
#define CHROME_TRACER_STREAM_TRACER_H_

#include "chrome_tracer/tracer_interface.h"

namespace chrome_tracer {

class ChromeStreamTracer : public ITracer {
 public:
  ChromeStreamTracer();
  ChromeStreamTracer(std::string name);

  bool HasStream(std::string stream);
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
  std::chrono::system_clock::time_point anchor_;
  mutable std::mutex lock_;
};

}  // namespace chrome_tracer

#endif  // CHROME_TRACER_STREAM_TRACER_H_