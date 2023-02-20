#include "chrome_tracer/tracer.h"

#include <iostream>
#include <fstream>
#include <chrono>

namespace chrome_tracer {

namespace {

Json::Value GenerateInstantEvent(
    std::string name,
    int pid,
    std::chrono::system_clock::time_point timestamp,
    std::chrono::system_clock::time_point anchor) {
  Json::Value result;
  result["name"] = name;
  result["ph"] = "i";  // Instant event;
  result["ts"] = std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count();
  result["pid"] = pid;
  return result;
}

Json::Value GenerateProcessMetaEvent(
    std::string name, 
    int pid) {
  Json::Value result;
  result["name"] = "process_name";
  result["ph"] = "M";  // Metadata event
  result["pid"] = pid;
  Json::Value args;
  args["name"] = name;
  result["args"] = args;
  return result;
}

Json::Value GenerateBeginEvent(
    std::string name,
    int pid,
    std::chrono::system_clock::time_point timestamp,
    std::chrono::system_clock::time_point anchor) {
  Json::Value result;
  result["name"] = name;
  result["ph"] = "B";
  result["ts"] = std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count();
  result["pid"] = pid;
  return result;
}

Json::Value GenerateEndEvent(
    std::string name,
    int pid,
    std::chrono::system_clock::time_point timestamp,
    std::chrono::system_clock::time_point anchor) {
  Json::Value result;
  result["name"] = name;
  result["ph"] = "E";
  result["ts"] = std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count();
  result["pid"] = pid;
  return result;
}

std::pair<Json::Value, Json::Value> GenerateDurationEvent(
    std::string name,
    int pid,
    std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point> duration,
    std::chrono::system_clock::time_point anchor) {
  return std::make_pair(
      GenerateBeginEvent(
          name, pid, duration.first, anchor), 
      GenerateEndEvent(
          name, pid, duration.second, anchor));
}

}  // anonymous namespace
  
ChromeTracer::ChromeTracer(std::string name) {
  this->name_ = name;
  anchor_ = std::chrono::system_clock::now();
}

bool ChromeTracer::HasStream(std::string stream) {
  std::lock_guard<std::mutex> lock(lock_);
  if (event_table_.find(stream) == event_table_.end()) {
    return false;
  }
  return true;
}
void ChromeTracer::AddStream(std::string stream) {
  if (HasStream(stream)) {
    std::cerr << "The given stream already exists." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);
  if (!event_table_.emplace(stream, std::map<std::string, Event>()).second) {
    std::cerr << "Failed to add a stream.";
    abort();
  }
}

bool ChromeTracer::HasEvent(std::string stream, std::string event) {
  if (!HasStream(stream)) {
    std::cerr << "The given stream does not exists." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);
  auto& events = event_table_[stream];
  if (events.find(event) == events.end()) {
    return false;
  }
  return true;
}

void ChromeTracer::BeginEvent(std::string stream, std::string event) {
  if (HasEvent(stream, event)) {
    std::cerr << "The given event already exists." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);
  auto& events = event_table_[stream];
  if (!events.emplace(event, Event(event)).second) {
    std::cerr << "Failed to start an event." << std::endl;
    abort();
  }
}

void ChromeTracer::EndEvent(std::string stream, std::string event) {
  if (!HasEvent(stream, event)) {
    std::cerr << "The given event does not exists." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);
  auto events = event_table_.find(stream)->second;
  auto new_event = events.find(event)->second;
  new_event.Finish();
  events.erase(event);
  events.emplace(event, new_event);
  event_table_[stream] = events;
}

bool ChromeTracer::Validate() const {
  std::lock_guard<std::mutex> lock(lock_);
  for (auto const& stream : event_table_) {
    for (auto const& events : stream.second) {
      if (events.second.GetStatus() != Event::EventStatus::Finished) {
        std::cerr << stream.first << " " << events.second.name;
        return false;
      }
    }
  }
  return true;
}

// Returns the json string.
std::string ChromeTracer::Dump() const {
  if (!Validate()) {
    std::cerr << "There is unfinished event." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);

  std::map<std::string, int> stream_pid_map;
  int i = 1;
  for (auto stream_name : event_table_) {
    stream_pid_map[stream_name.first] = i;
    i++;
  }

  Json::Value result;
  Json::Value trace_events;
  // 1. Start event per stream
  for (auto const& stream : event_table_) {
    std::string stream_name = stream.first;
    trace_events.append(
        GenerateInstantEvent(
           "Start",
           stream_pid_map[stream_name],
           anchor_, anchor_));
  }

  // 2. Metadata event per stream
  for (auto const& stream : event_table_) {
    std::string stream_name = stream.first;
    trace_events.append(
        GenerateProcessMetaEvent(
            stream_name,
            stream_pid_map[stream_name]));
  }

  // 3. Duration event per events
  for (auto const& stream : event_table_) {
    for (auto const& event : stream.second) {
      auto dur_events = GenerateDurationEvent(
          event.first,
          stream_pid_map[stream.first],
          std::make_pair(event.second.start, event.second.end),
          anchor_);
      trace_events.append(dur_events.first);
      trace_events.append(dur_events.second);
    }
  }

  result["traceEvents"] = trace_events;
  
  return Json::FastWriter().write(result);
}

// Dump the json string to the file path.
void ChromeTracer::Dump(std::string path) const {
  std::ofstream fout;
  fout.open(path);
  if (fout.is_open() == false) {
    std::cerr << "Cannot open the file path." << std::endl;
    abort();
  }
  fout << Dump();
  fout.close();
}

std::string ChromeTracer::Summary() const {
  return "";
}

void ChromeTracer::Clear() {
  for (auto& stream : event_table_) {
    stream.second.clear();
  }
  anchor_ = std::chrono::system_clock::now();
}

}  // namespace chrome_tracer