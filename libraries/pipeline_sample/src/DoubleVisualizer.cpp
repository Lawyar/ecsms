#include "DoubleVisualizer.h"

#include <iostream>

using namespace std;

DoubleVisualizer::DoubleVisualizer(
    ConsumerStrategy strategy,
    std::shared_ptr<InStageConnection<double>> connection)
    : ConsumerStage(stageName, strategy, connection) {
}

void DoubleVisualizer::consume(std::shared_ptr<double> inData) {
  cout << *inData << endl;
  releaseConsumptionData(inData);
}
