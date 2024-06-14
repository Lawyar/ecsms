#include "Int32Visualizer.h"

#include <fstream>
#include <iostream>

using namespace std;

Int32Visualizer::Int32Visualizer(ConsumptionStrategy strategy,
    std::shared_ptr<InStageConnection<int32_t>> connection)
    : ConsumerStage(stageName, strategy, connection) {
}

void Int32Visualizer::consume(std::shared_ptr<int32_t> inData) {
  std::ofstream file(stageName + std::string(".txt"));
  file << "Int32Visualizer: " << *inData << endl;
  cout << "Int32Visualizer: " << *inData << endl;
  dataConsumed(inData);
}
