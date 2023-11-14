#include "chrome_tracer/stream_tracer.h"

namespace chrome_tracer {
  
ChromeStreamTracer::ChromeStreamTracer() {
  anchor_ = std::chrono::system_clock::now();
}

ChromeStreamTracer::ChromeStreamTracer(std::string name) : name_(name) {
  anchor_ = std::chrono::system_clock::now();
}

bool ChromeStreamTracer::HasStream(std::string stream) {
  std::lock_guard<std::mutex> lock(lock_);
  return true;
}

void ChromeStreamTracer::AddStream(std::string stream) {
  std::lock_guard<std::mutex> lock(lock_);
}

bool ChromeStreamTracer::HasEvent(std::string stream, int32_t handle) {
  std::lock_guard<std::mutex> lock(lock_);
  return true;
}

void ChromeStreamTracer::MarkEvent(std::string stream, std::string event) {
  std::lock_guard<std::mutex> lock(lock_);
}

int32_t ChromeStreamTracer::BeginEvent(std::string stream, std::string event) {
  std::lock_guard<std::mutex> lock(lock_);
  return 0;
}

void ChromeStreamTracer::EndEvent(std::string stream, int32_t handle, std::string args) {
  std::lock_guard<std::mutex> lock(lock_);
}

bool ChromeStreamTracer::Validate() const {
  std::lock_guard<std::mutex> lock(lock_);
  return true;
}

std::string ChromeStreamTracer::Dump() const {
  std::lock_guard<std::mutex> lock(lock_);
  return "";
}

void ChromeStreamTracer::Dump(std::string path) const {
  std::lock_guard<std::mutex> lock(lock_);
}

std::string ChromeStreamTracer::Summary() const {
  std::lock_guard<std::mutex> lock(lock_);
  return "";
}

void ChromeStreamTracer::Clear() {
  std::lock_guard<std::mutex> lock(lock_);
}

}  // namepsace chrome_tracer