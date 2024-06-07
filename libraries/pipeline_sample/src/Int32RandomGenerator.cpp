#include "Int32RandomGenerator.h"
#include "PipelineRegistry.h"

#include <random>
#include <thread>

using namespace std;

Int32RandomGenerator::Int32RandomGenerator(
    std::shared_ptr<OutStageConnection<int32_t>> outConnection)
    : ProducerStage(stageName, outConnection) {
  PipelineRegistry::Instance().registerProducer<Int32RandomGenerator>(
      stageName);
}

void Int32RandomGenerator::produce(std::shared_ptr<int32_t> outData) {
  this_thread::sleep_for(chrono::milliseconds(500));
  *outData = rand() % 100;
  releaseProductionData(outData, true);
}
