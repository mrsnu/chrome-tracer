#include "tracer.h"

#include <iostream>
#include <fstream>
#include <chrono>

namespace chrome_tracer {

namespace {

std::string GenerateInstantEvent(
    std::string name,
    int pid,
    std::chrono::system_clock::time_point timestamp,
    std::chrono::system_clock::time_point anchor) {
  std::string result = "{";
  result += "\"name\": \"" + name + "\", ";
  result += "\"ph\": \"i\", ";
  result += "\"ts\": " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count()) + ", ";
  result += "\"pid\": " + std::to_string(pid);
  result += "}";
  return result;
}

std::string GenerateProcessMetaEvent(
    std::string name, 
    int pid) {
  std::string result = "{";
  result += "\"name\": \"process_name\", ";
  result += "\"ph\": \"M\", ";
  result += "\"pid\": " + std::to_string(pid) + ", ";
  result += "\"args\": {";
    result += "\"name\": \"" + name + "\"";
  result += "}";
  result += "}";
  return result;
}

std::string GenerateThreadMetaEvent(
    std::string name,
    int pid,
    int tid) {
  std::string result = "{";
  result += "\"name\": \"thread_name\", ";
  result += "\"ph\": \"M\", ";
  result += "\"pid\": " + std::to_string(pid) + ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"args\": {";
  result += "\"name\": \"" + name + "\"";
  result += "}";
  result += "}";
  return result;
}

std::string GenerateBeginEvent(
    std::string name,
    std::string metadata,
    int pid,
    int tid,
    std::chrono::system_clock::time_point timestamp,
    std::chrono::system_clock::time_point anchor) {
  std::string name_metadata = name + " (" + metadata + ")";
  std::string result = "{";
  result += "\"name\": \"" + name_metadata + "\", ";
  result += "\"ph\": \"B\", ";
  result += "\"ts\": " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count()) + ", ";
  result += "\"pid\": " + std::to_string(pid) + ", ";
  result += "\"tid\": " + std::to_string(tid);
  result += "}";
  return result;
}

std::string GenerateEndEvent(
    std::string name,
    std::string metadata,
    int pid,
    int tid,
    std::chrono::system_clock::time_point timestamp,
    std::chrono::system_clock::time_point anchor) {
  std::string name_metadata = name + " (" + metadata + ")";
  std::string result = "{";
  result += "\"name\": \"" + name_metadata + "\", ";
  result += "\"ph\": \"E\", ";
  result += "\"ts\": " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count()) + ", ";
  result += "\"pid\": " + std::to_string(pid) + ", ";
  result += "\"tid\": " + std::to_string(tid);
  result += "}";
  return result;
}

std::pair<std::string, std::string> GenerateDurationEvent(
    std::string name,
    std::string metadata,
    int pid,
    int tid,
    std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point> duration,
    std::chrono::system_clock::time_point anchor) {
  return std::make_pair(
      GenerateBeginEvent(
          name, metadata, pid, tid, duration.first, anchor), 
      GenerateEndEvent(
          name, metadata, pid, tid, duration.second, anchor));
}

}  // anonymous namespace
  
ChromeTracer::ChromeTracer(std::string name) {
  this->name_ = name;
  anchor_ = std::chrono::system_clock::now();
  count_ = 0;
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
  if (!event_table_.emplace(
      stream, 
      std::map<int32_t, Event>()).second) {
    std::cerr << "Failed to add a stream.";
    abort();
  }
}

bool ChromeTracer::HasEvent(std::string stream, int32_t handle) {
  if (!HasStream(stream)) {
    std::cerr << "The given stream does not exists." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);
  auto& events = event_table_[stream];
  if (events.find(handle) == events.end()) {
    return false;
  }
  return true;
}

void ChromeTracer::MarkEvent(
    std::string stream,
    std::string name,
    std::string metadata) {
  std::lock_guard<std::mutex> lock(lock_);
  auto& events = event_table_[stream];
  if (!events.emplace(count_, Event(name, metadata, std::chrono::system_clock::now(), Event::EventStatus::Instantanous)).second) {
    std::cerr << "Failed to start an event." << std::endl;
    abort();
  }
  count_++;
}

int32_t ChromeTracer::BeginEvent(
    std::string stream,
    std::string event_name,
    std::string metadata) {
  std::lock_guard<std::mutex> lock(lock_);

  auto& events = event_table_[stream];
  if (!events.emplace(count_, Event(event_name, metadata, std::chrono::system_clock::now())).second) {
    std::cerr << "Failed to start an event." << std::endl;
    abort();
  }
  return count_++;
}

void ChromeTracer::EndEvent(
    std::string stream,
    int32_t handle) {
  if (!HasEvent(stream, handle)) {
    std::cerr << "The given event does not exists." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);
  auto events = event_table_.find(stream)->second;
  auto new_event = events.find(handle)->second;
  new_event.Finish();
  events.erase(handle);
  events.emplace(handle, new_event);
  event_table_.erase(stream);
  event_table_.emplace(stream, events);
  // event_table_[stream] = events;
}

bool ChromeTracer::Validate() const {
  std::lock_guard<std::mutex> lock(lock_);
  for (auto const& stream : event_table_) {
    for (auto const& events : stream.second) {
      if (events.second.GetStatus() == Event::EventStatus::Running) {
        std::cerr << stream.first << " " << events.second.name;
        return false;
      }
    }
  }
  return true;
}

// Returns the json string.
std::string ChromeTracer::Dump(bool force) const {
  if (!force && !Validate()) {
    std::cerr << "There is unfinished event." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);

  std::map<std::string, int> stream_pid_map;
  std::map<std::string, int> event_tid_map;
  int pid = 1;
  int tid = 0;
  for (auto stream : event_table_) {
    stream_pid_map[stream.first] = pid;
    pid++;
    for (auto const& it : stream.second) {
      auto const& event = it.second;
      if (event_tid_map.find(event.name) == event_tid_map.end()) {
        event_tid_map[event.name] = tid;
        tid++;        
      }
    }
  }

  std::string result = "{";
  std::string trace_events = "[";
  // 1. Start event per stream
  for (auto const& stream : event_table_) {
    std::string stream_name = stream.first;
    trace_events += 
        GenerateInstantEvent(
           "Start",
           stream_pid_map[stream_name],
           anchor_, anchor_) + ",";
  }

  // 2. Metadata event per process and per thread
  for (auto const& stream : event_table_) {
    std::string stream_name = stream.first;
    trace_events += 
        GenerateProcessMetaEvent(
            stream_name,
            stream_pid_map[stream_name]) + ",";
    for (auto const& event : stream.second) {
      trace_events +=
          GenerateThreadMetaEvent(
              event.second.name,
              stream_pid_map[stream_name],
              event_tid_map[event.second.name]) + ",";
    }
  }

  // 3. Duration event per events
  for (auto const& stream : event_table_) {
    for (auto const& event : stream.second) {
      switch (event.second.GetStatus()) {
        case Event::EventStatus::Finished: {
          auto dur_events = GenerateDurationEvent(
              event.second.name,
              event.second.metadata,
              stream_pid_map[stream.first],
              event_tid_map[event.second.name],
              std::make_pair(event.second.start, event.second.end),
              anchor_);
          trace_events += dur_events.first + ",";
          trace_events += dur_events.second + ",";
        } break;
        case Event::EventStatus::Instantanous: {
          auto inst_event = GenerateInstantEvent(
              event.second.name,
              stream_pid_map[stream.first],
              event.second.start,
              anchor_);
          trace_events += inst_event + ",";
        } break;
        case Event::EventStatus::Running: {
          if (force) {
            break;
          }
        }
        default: {
          std::cerr << "Invalid event state.";
          abort();
        }
      }
    }
  }
  trace_events = trace_events.substr(0, trace_events.size() - 1) + "]";

  result += "\"traceEvents\": " + trace_events;

  result += "}";
  
  return result;
}

// Dump the json string to the file path.
void ChromeTracer::Dump(std::string path, bool force) const {
  std::ofstream fout;
  fout.open(path);
  if (fout.is_open() == false) {
    std::cerr << "Cannot open the file path." << std::endl;
    abort();
  }
  fout << Dump(force);
  fout.close();
}

std::string ChromeTracer::Summary() const {
  return "";
}

}  // namespace chrome_tracer