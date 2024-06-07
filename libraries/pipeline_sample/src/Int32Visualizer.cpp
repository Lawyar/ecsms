#include "Int32Visualizer.h"
#include "PipelineRegistry.h"

#include <iostream>

using namespace std;

Int32Visualizer::Int32Visualizer(ConsumerStrategy strategy,
    std::shared_ptr<InStageConnection<int32_t>> connection)
    : ConsumerStage(stageName, strategy, connection) {
  PipelineRegistry::Instance().registerConsumer<Int32Visualizer>(stageName);
}

void Int32Visualizer::consume(std::shared_ptr<int32_t> inData) {
  cout << *inData << endl;
  releaseConsumptionData(inData);
}
