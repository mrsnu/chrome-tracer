#include "chrome_tracer/tracer.h"

#include <iostream>
#include <thread>

void BeginEndEvent(chrome_tracer::ChromeTracer& tracer, std::string event_name) {
    tracer.BeginEvent("DefaultStream", event_name);
    tracer.EndEvent("DefaultStream", event_name);
}

int main() {
    chrome_tracer::ChromeTracer tracer("TestThread");
    std::vector<std::thread> threads;

    tracer.AddStream("DefaultStream");

    for (int i = 0; i < 32; i++) {
        std::string event_name = std::to_string(i);
        threads.push_back(std::thread(BeginEndEvent, std::ref(tracer), event_name));
    }

    for (int i = 0; i < 32; i++) {
        threads[i].join();
    }

    std::cout << tracer.Dump() << std::endl;
    tracer.Dump("test.json");

    return 0;
}