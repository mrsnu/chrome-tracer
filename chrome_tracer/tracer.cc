#include "chrome_tracer/tracer.h"

#include <iostream>
#include <fstream>

namespace chrome_tracer {
  
ChromeTracer::ChromeTracer(std::string name) {
  this->name_ = name;
}

bool ChromeTracer::HasStream(std::string stream) {
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
  auto& events = event_table_[stream];
  if (events.find(event) == events.end()) {
    return false;
  }
  return true;
}

void ChromeTracer::StartEvent(std::string stream, std::string event) {
  if (HasEvent(stream, event)) {
    std::cerr << "The given event already exists." << std::endl;
    abort();
  }
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
  auto events = event_table_.find(stream)->second;
  auto new_event = events.find(event)->second;
  new_event.Finish();
  events.erase(event);
  events.emplace(event, new_event);
  event_table_[stream] = events;
}

bool ChromeTracer::Validate() const {
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

  return "Test String";
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
  
}

}  // namespace chrome_tracer