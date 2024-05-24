#include "int32_random_generator.h"

#include <random>
#include <thread>

using namespace std;

Int32RandomGenerator::Int32RandomGenerator(
    const std::string_view stageName,
    std::shared_ptr<OutStageConnection<int32_t>> outConnection)
    : ProducerStage(stageName, outConnection) {}

void Int32RandomGenerator::produce(std::shared_ptr<int32_t> outData) {
  this_thread::sleep_for(chrono::milliseconds(500));
  *outData = rand() % 100;
  releaseProducerTask(outData, true);
}
