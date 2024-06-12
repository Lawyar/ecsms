#include "DoubleVisualizer.h"
#include "Int32RandomGenerator.h"
#include "Int32ToDoubleConverter.h"
#include "Int32Visualizer.h"
#include "Pipeline.h"
#include "PipelineRegistry.h"
#include "SteadyClock.h"

#include <iostream>

using namespace std;

int main() {
  try {
    auto& registry = PipelineRegistry::Instance();

    auto int32Connection =
        registry.constructProducerConnection("Int32RandomGenerator", 128);
    auto int32Generator =
        registry.constructProducer("Int32RandomGenerator", int32Connection);
    auto int32Visualizer = registry.constructConsumer(
        "Int32Visualizer", ConsumptionStrategy::fifo, int32Connection);

    auto doubleConnection =
        registry.constructProducerConnection("Int32ToDoubleConverter", 128);
    auto int32ToDouble = registry.constructConsumerAndProducer(
        "Int32ToDoubleConverter", ConsumptionStrategy::fifo, int32Connection,
        doubleConnection);
    auto doubleVisualizer = registry.constructConsumer(
        "DoubleVisualizer", ConsumptionStrategy::fifo, doubleConnection);

    Pipeline pipeline;
    pipeline.addConnection(int32Connection);
    pipeline.addConnection(doubleConnection);

    pipeline.addStage(int32Generator);
    pipeline.addStage(int32Visualizer);
    pipeline.addStage(int32ToDouble);
    pipeline.addStage(doubleVisualizer);

    pipeline.run();

    SteadyClock::waitForMs(1000);

    pipeline.shutdown();

    while (true) {
    }
  } catch (std::exception& ex) {
    cerr << ex.what() << endl;
  } catch (...) {
    cerr << "unhandled exception in main thread" << endl;
  }
}
