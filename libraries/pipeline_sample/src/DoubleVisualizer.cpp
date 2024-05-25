#include "DoubleVisualizer.h"

#include <iostream>

using namespace std;

DoubleVisualizer::DoubleVisualizer(
    std::shared_ptr<InStageConnection<double>> connection)
    : ConsumerStage(stageName, TaskRetrieveStrategy::oldest, connection) {}

void DoubleVisualizer::consume(std::shared_ptr<double> inData) {
  cout << *inData << endl;
  releaseConsumerTask(inData);
}
