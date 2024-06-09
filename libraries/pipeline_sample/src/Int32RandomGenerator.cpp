#include "Int32RandomGenerator.h"

#include <random>
#include <thread>

using namespace std;

Int32RandomGenerator::Int32RandomGenerator(
    std::shared_ptr<OutStageConnection<int32_t>> outConnection)
    : ProducerStage(stageName, outConnection) {
}

void Int32RandomGenerator::produce(std::shared_ptr<int32_t> outData) {
  this_thread::sleep_for(chrono::milliseconds(10));
  *outData = rand() % 100;
  releaseProductionData(outData, true);
}
