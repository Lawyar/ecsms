#include "int32_random_generator_pipeline_factory.h"
#include "InOutStageConnection.h"
#include "int32_random_generator.h"
#include "int32_visualizer.h"

using namespace std;

shared_ptr<Pipeline> int32_random_generator_pipeline_factory::create() {
  auto pipeline = make_shared<Pipeline>();
  auto connection = createConnection();

  auto producer =
      make_shared<Int32RandomGenerator>("Int32RandomGenerator", connection);
  auto consumer = make_shared<Int32Visualizer>(
      "Int32Visualizer", TaskRetrieveStrategy::oldest, connection);

  pipeline->add_connection(connection);
  pipeline->add_stage(producer);
  pipeline->add_stage(consumer);

  return pipeline;
}

std::shared_ptr<InOutStageConnection<int32_t>>
int32_random_generator_pipeline_factory::createConnection() {
  std::vector<shared_ptr<int32_t>> connectionData(connection_capacity);
  for (auto &it : connectionData)
    it = make_shared<int32_t>();

  return make_shared<InOutStageConnection<int32_t>>(connectionData);
}
