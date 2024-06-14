#include "DoubleVisualizer.h"

#include <iostream>
#include <fstream>

using namespace std;

DoubleVisualizer::DoubleVisualizer(
    ConsumptionStrategy strategy,
    std::shared_ptr<InStageConnection<double>> connection)
    : ConsumerStage(stageName, strategy, connection) {
}

void DoubleVisualizer::consume(std::shared_ptr<double> inData) {
  std::ofstream file(stageName + std::string(".txt"), std::ios::app);
  file << *inData << ' ' << endl;
  cout << "DoubleVisualizer: " << *inData << endl;
  dataConsumed(inData);
}
