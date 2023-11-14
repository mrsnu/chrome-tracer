#ifndef CHROME_TRACER_TRACER_INTERFACE_H_
#define CHROME_TRACER_TRACER_INTERFACE_H_

#include "chrome_tracer/event.h"

namespace chrome_tracer {

class ITracer {
 public:
  virtual bool HasStream(std::string stream) = 0;
  virtual void AddStream(std::string stream) = 0;
  virtual bool HasEvent(std::string stream, int32_t handle) = 0;
  virtual void MarkEvent(std::string stream, std::string event) = 0;
  virtual int32_t BeginEvent(std::string stream, std::string event) = 0;
  virtual void EndEvent(std::string stream, int32_t handle, std::string args = "") = 0;
  virtual bool Validate() const = 0;
  virtual std::string Dump() const = 0;
  virtual void Dump(std::string path) const = 0;
  virtual std::string Summary() const = 0;
  virtual void Clear() = 0;
};

}  // namespace chrome_tracer

#endif  // CHROME_TRACER_TRACER_INTERFACE_H_