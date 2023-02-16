#ifndef CHROME_TRACER_TRACER_H_

#include <string>
#include <map>
#include <vector>
#include <chrono>

#include "chrome_tracer/event.h"

#include <json/json.h>

namespace chrome_tracer {

class ChromeTracer {
 public: 
  ChromeTracer(std::string name);

  bool HasStream(std::string stream);
  void AddStream(std::string stream);

  bool HasEvent(std::string stream, std::string event);
  void StartEvent(std::string stream, std::string event);
  void EndEvent(std::string stream, std::string event);

  bool Validate() const;

  std::string Dump() const;
  void Dump(std::string path) const;
  std::string Summary() const;

 private:
  std::string name_;
  std::map<std::string, std::map<std::string, Event>> event_table_;
};

}  // namespace chrome_tracer

#endif  // CHROME_TRACER_TRACER_H_