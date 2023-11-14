#include <chrono>
#include <string>

namespace chrome_tracer {

std::string GenerateInstantEvent(
    std::string name, int pid, int tid,
    std::chrono::system_clock::time_point timestamp,
    std::chrono::system_clock::time_point anchor) {
  std::string result = "{";
  result += "\"name\": \"" + name + "\", ";
  result += "\"ph\": \"i\", ";
  result +=
      "\"ts\": " +
      std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
                         timestamp - anchor)
                         .count()) +
      ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"pid\": " + std::to_string(pid);
  result += "}";
  return result;
}

std::string GenerateProcessMetaEvent(std::string name, std::string meta_name,
                                     int pid, int tid) {
  std::string result = "{";
  result += "\"name\": \"" + meta_name + "\", ";
  result += "\"ph\": \"M\", ";
  result += "\"pid\": " + std::to_string(pid) + ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"args\": {";
  result += "\"name\": \"" + name + "\"";
  result += "}";
  result += "}";
  return result;
}

std::string GenerateBeginEvent(std::string name, int pid, int tid,
                               std::chrono::system_clock::time_point timestamp,
                               std::chrono::system_clock::time_point anchor) {
  std::string result = "{";
  result += "\"name\": \"" + name + "\", ";
  result += "\"ph\": \"B\", ";
  result +=
      "\"ts\": " +
      std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
                         timestamp - anchor)
                         .count()) +
      ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"pid\": " + std::to_string(pid);
  result += "}";
  return result;
}

std::string GenerateEndEvent(std::string name, int pid, int tid,
                             std::chrono::system_clock::time_point timestamp,
                             std::chrono::system_clock::time_point anchor,
                             std::string args) {
  std::string result = "{";
  result += "\"name\": \"" + name + "\", ";
  result += "\"ph\": \"E\", ";
  result +=
      "\"ts\": " +
      std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
                         timestamp - anchor)
                         .count()) +
      ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"pid\": " + std::to_string(pid);
  if (args != "") {
    result += ", \"args\": " + args;
  }
  result += "}";
  return result;
}

std::pair<std::string, std::string> GenerateDurationEvent(
    std::string name, int pid, int tid,
    std::pair<std::chrono::system_clock::time_point,
              std::chrono::system_clock::time_point>
        duration,
    std::chrono::system_clock::time_point anchor, std::string args) {
  return std::make_pair(
      GenerateBeginEvent(name, pid, tid, duration.first, anchor),
      GenerateEndEvent(name, pid, tid, duration.second, anchor, args));
}

}  // namespace chrome_tracer