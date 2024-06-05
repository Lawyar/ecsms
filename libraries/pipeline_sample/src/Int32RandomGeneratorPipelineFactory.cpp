#include "Int32RandomGeneratorPipelineFactory.h"
#include "Int32RandomGenerator.h"
#include "Int32Visualizer.h"

using namespace std;

shared_ptr<Pipeline> Int32RandomGeneratorPipelineFactory::create() {
  auto pipeline = make_shared<Pipeline>();
  auto connection = make_shared<InOutStageConnection<int32_t>>(connectionCapacity);

  auto producer =
      make_shared<Int32RandomGenerator>(connection);
  auto consumer = make_shared<Int32Visualizer>(ConsumerStrategy::consumeNewest, connection);

  pipeline->addConnection(connection);
  pipeline->addStage(producer);
  pipeline->addStage(consumer);

  return pipeline;
}
