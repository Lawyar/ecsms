#include <gtest/gtest.h>

#include <fstream>

#include "PipelineToYaml.h"
#include "PipelineRegistry.h"

using namespace std;
namespace fs = filesystem;
using namespace testing;

class PipelineToYaml_test : public Test {
 public:
  static void SetUpTestSuite() {
    auto registry = PipelineRegistry::Instance();
    
    auto int32Connection =
        registry.constructProducerConnection("Int32RandomGenerator", 128);

    auto int32Generator =
        registry.constructProducer("Int32RandomGenerator", int32Connection);
    int32Generator->setId("Int 32 generator");

    auto int32Visualizer = registry.constructConsumer(
        "Int32Visualizer", ConsumptionStrategy::fifo, int32Connection);
    int32Visualizer->setId("Int 32 visualizer");
    int32Visualizer->setParentId("Int 32 generator");

    auto doubleConnection =
        registry.constructProducerConnection("Int32ToDoubleConverter", 128);

    auto int32ToDouble = registry.constructConsumerAndProducer(
        "Int32ToDoubleConverter", ConsumptionStrategy::fifo, int32Connection,
        doubleConnection);
    int32ToDouble->setId("Int 32 to double");
    int32ToDouble->setParentId("Int 32 generator");

    auto doubleVisualizer = registry.constructConsumer(
        "DoubleVisualizer", ConsumptionStrategy::fifo, doubleConnection);
    doubleVisualizer->setId("Double visualizer");
    doubleVisualizer->setParentId("Int 32 to double");

    pipeline->addConnection(int32Connection);
    pipeline->addConnection(doubleConnection);

    pipeline->addStage(int32Generator);
    pipeline->addStage(int32Visualizer);
    pipeline->addStage(int32ToDouble);
    pipeline->addStage(doubleVisualizer);
  }

  static constexpr auto ExpectedPipelineStr =
      "Int32RandomGenerator:\n"
      "  id: Int 32 generator\n"
      "  type: producer\n"
      "\n"
      "\n"
      "Int32Visualizer:\n"
      "  id: Int 32 visualizer\n"
      "  type: consumer\n"
      "  strategy: fifo\n"
      "  parentId: Int 32 generator\n"
      "\n"
      "\n"
      "Int32ToDoubleConverter:\n"
      "  id: Int 32 to double\n"
      "  type: producerConsumer\n"
      "  strategy: fifo\n"
      "  parentId: Int 32 generator\n"
      "\n"
      "\n"
      "DoubleVisualizer:\n"
      "  id: Double visualizer\n"
      "  type: consumer\n"
      "  strategy: fifo\n"
      "  parentId: Int 32 to double\n"
      "\n";

  static constexpr auto PipelineFilePath = "pipeline.yaml";

 public:
  static shared_ptr<Pipeline> pipeline;
};

decltype(PipelineToYaml_test::pipeline) PipelineToYaml_test::pipeline =
    make_shared<Pipeline>();

TEST_F(PipelineToYaml_test, PipelineToYaml_toFileWorks) {
  static constexpr auto filePath = "pipeline.yaml";
  PipelineToYaml::serializeToFile(pipeline, filePath);

  ifstream file(filePath);
  string piepelineStr((istreambuf_iterator<char>(file)),
              istreambuf_iterator<char>());
  file.close();
  fs::remove(filePath);

  ASSERT_EQ(piepelineStr, ExpectedPipelineStr);
}

TEST_F(PipelineToYaml_test, PipelineToYaml_toStringWorks) {
  auto pipelineStr = PipelineToYaml::serializeToString(pipeline);
  ASSERT_EQ(pipelineStr, ExpectedPipelineStr);
}
