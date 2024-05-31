#include "DoubleVisualizer.h"

#include <iostream>

using namespace std;

DoubleVisualizer::DoubleVisualizer(
    std::shared_ptr<InStageConnection<double>> connection)
    : ConsumerStage(stageName, ConsumerStrategy::consumeOldest, connection) {}

void DoubleVisualizer::consume(std::shared_ptr<double> inData) {
  cout << *inData << endl;
  releaseConsumptionData(inData);
}
