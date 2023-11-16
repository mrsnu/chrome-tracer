#include "chrome_tracer/stream_tracer.h"

#include "chrome_tracer/utils.h"

namespace chrome_tracer {

ChromeStreamTracer::ChromeStreamTracer(std::string path)
    : path_(path), pid_(GetNextPid()) {
  anchor_ = std::chrono::system_clock::now();
  file_.open(path);
  if (file_.is_open() == false) {
    std::cerr << "Cannot open the file path." << std::endl;
    abort();
  }
  file_ << "{";
  file_ << "\"traceEvents\": [";
  file_ << GenerateProcessMetaEvent(name_, "process_name", pid_, 0) + ",";
}

ChromeStreamTracer::ChromeStreamTracer(std::string name, std::string path)
    : ChromeStreamTracer(path) {
  this->name_ = name;
}

ChromeStreamTracer::~ChromeStreamTracer() {
  file_.seekp(-1, std::ios_base::end);
  file_ << "]";
  file_ << "}";
  file_.close();
}

bool ChromeStreamTracer::HasStream(std::string stream) const {
  std::lock_guard<std::mutex> lock(lock_);
  if (stream_table_.find(stream) == stream_table_.end()) {
    return false;
  }
  return true;
}

void ChromeStreamTracer::AddStream(std::string stream) {
  if (HasStream(stream)) {
    std::cerr << "The given stream already exists." << std::endl;
    abort();
  }

  std::lock_guard<std::mutex> lock(lock_);
  file_ << GenerateProcessMetaEvent(stream, "thread_name", pid_,
                                    stream_table_.size()) +
               ",";
  file_ << GenerateInstantEvent("Start", pid_, stream_table_.size(), anchor_,
                                anchor_) +
               ",";
  stream_table_[stream] = stream_table_.size();
}

bool ChromeStreamTracer::HasEvent(std::string stream, int32_t handle) {
  std::cout << "Warning: Stream tracer does not support HasEvent." << std::endl;
  return true;
}

void ChromeStreamTracer::MarkEvent(std::string stream, std::string name) {
  // GenerateInstantEvent
  auto event = Event(name, Event::EventStatus::Instantaneous);
  file_ << GenerateInstantEvent(event.name, pid_, stream_table_[stream],
                                anchor_, event.start) +
               ",";
}

int32_t ChromeStreamTracer::BeginEvent(std::string stream, std::string name) {
  std::lock_guard<std::mutex> lock(lock_);
  event_table_.emplace(std::piecewise_construct, std::forward_as_tuple(count_),
                       std::forward_as_tuple(name));
  file_ << GenerateBeginEvent(name, pid_, stream_table_.at(stream),
                              event_table_.at(count_).start, anchor_) +
               ",";
  return count_++;
}

void ChromeStreamTracer::EndEvent(std::string stream, int32_t handle,
                                  std::string args) {
  std::lock_guard<std::mutex> lock(lock_);
  auto event = event_table_.find(handle)->second;
  event.args = args;
  event.Finish();
  file_ << GenerateEndEvent(event.name, pid_, stream_table_[stream], event.end,
                            anchor_, args) +
               ",";
  event_table_.erase(handle);
}

bool ChromeStreamTracer::Validate() const { return true; }

std::string ChromeStreamTracer::Dump() const { return ""; }

void ChromeStreamTracer::Dump(std::string path) const { return; }

std::string ChromeStreamTracer::Summary() const { return ""; }

void ChromeStreamTracer::Clear() {
  std::lock_guard<std::mutex> lock(lock_);
  event_table_.clear();
  stream_table_.clear();
}

}  // namespace chrome_tracer