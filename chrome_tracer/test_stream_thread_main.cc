#include <iostream>
#include <thread>
#include <vector>

#include "chrome_tracer/stream_tracer.h"

std::map<std::string, int32_t> handles;

void BeginEvent(chrome_tracer::ChromeStreamTracer& tracer, std::string event_name) {
  handles[event_name] = tracer.BeginEvent("DefaultStream", event_name);
}

void EndEvent(chrome_tracer::ChromeStreamTracer& tracer, std::string event_name) {
  tracer.EndEvent("DefaultStream", handles[event_name]);
}

int main() {
  chrome_tracer::ChromeStreamTracer tracer("test.json");
  std::vector<std::thread> start_threads;
  std::vector<std::thread> end_threads;

  tracer.AddStream("DefaultStream");

  for (int i = 0; i < 32; i++) {
    std::string event_name = std::to_string(i);
    start_threads.push_back(
        std::thread(BeginEvent, std::ref(tracer), event_name));
  }

  for (int i = 0; i < 32; i++) {
    start_threads[i].join();
    std::cout << "Thread " << i << " joined" << std::endl;
  }

  for (int i = 0; i < 32; i++) {
    std::string event_name = std::to_string(i);
    end_threads.push_back(std::thread(EndEvent, std::ref(tracer), event_name));
  }

  for (int i = 0; i < 32; i++) {
    end_threads[i].join();
    std::cout << "Thread " << i << " joined" << std::endl;
  }

  return 0;
}