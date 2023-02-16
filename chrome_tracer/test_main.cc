#include <string>

#include "chrome_tracer/tracer.h"

static const char* kAnalysisPipeline = "AnalysisPipeline";
static const char* kRenderingPipeline = "RenderingPipeline";

static const char* kEventAnalysis1 = "EventAnalysis1";
static const char* kEventAnalysis2 = "EventAnalysis2";
static const char* kEventAnalysis3 = "EventAnalysis3";
static const char* kEventAnalysis4 = "EventAnalysis4";
static const char* kEventAnalysis5 = "EventAnalysis5";

static const char* kEventRendering1 = "EventRendering1";
static const char* kEventRendering2 = "EventRendering2";
static const char* kEventRendering3 = "EventRendering3";
static const char* kEventRendering4 = "EventRendering4";
static const char* kEventRendering5 = "EventRendering5";

int main() {
  chrome_tracer::ChromeTracer tracer("test_tracer");
  
  tracer.AddStream(kAnalysisPipeline);
  tracer.AddStream(kRenderingPipeline);
  
  tracer.StartEvent(kAnalysisPipeline, kEventAnalysis1);
  tracer.StartEvent(kAnalysisPipeline, kEventAnalysis2);
  tracer.StartEvent(kAnalysisPipeline, kEventAnalysis3);
  tracer.StartEvent(kAnalysisPipeline, kEventAnalysis4);
  tracer.StartEvent(kAnalysisPipeline, kEventAnalysis5);

  tracer.EndEvent(kAnalysisPipeline, kEventAnalysis1);
  tracer.EndEvent(kAnalysisPipeline, kEventAnalysis2);
  tracer.EndEvent(kAnalysisPipeline, kEventAnalysis3);
  tracer.EndEvent(kAnalysisPipeline, kEventAnalysis4);
  tracer.EndEvent(kAnalysisPipeline, kEventAnalysis5);

  printf("Hello, world!\n");

  return 0;
}