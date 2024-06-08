#include "DoubleVisualizer.h"

#include <iostream>

using namespace std;

DoubleVisualizer::DoubleVisualizer(
    ConsumptionStrategy strategy,
    std::shared_ptr<InStageConnection<double>> connection)
    : ConsumerStage(stageName, strategy, connection) {
}

void DoubleVisualizer::consume(std::shared_ptr<double> inData) {
  cout << "DoubleVisualizer: " << *inData << endl;
  releaseConsumptionData(inData);
}
