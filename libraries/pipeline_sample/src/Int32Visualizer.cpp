#include "Int32Visualizer.h"

#include <iostream>

using namespace std;

Int32Visualizer::Int32Visualizer(
    std::shared_ptr<InStageConnection<int32_t>> connection)
    : ConsumerStage(stageName, TaskRetrieveStrategy::oldest, connection) {}

void Int32Visualizer::consume(std::shared_ptr<int32_t> inData) {
  cout << *inData << endl;
  releaseConsumerTask(inData);
}
