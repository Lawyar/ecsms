#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "SteadyClock.h"
#include "YamlToPipeline.h"

using namespace std;
namespace fs = filesystem;
using namespace testing;

class YamlToPipeline_test : public Test {
 public:
  static void SetUpTestSuite() {
    PipelineFile.open(PipelineFilePath);
    PipelineFile.write(PipelineStr, strlen(PipelineStr));
    PipelineFile.flush();
  }

  static void TearDownTestSuite() {
    PipelineFile.close();
    fs::remove(PipelineFilePath);
  }

  static constexpr auto PipelineStr =
      "Int32RandomGenerator:\n"
      "  id: \"Int 32 generator\"\n"
      "  type: producer\n"
      "\n"
      "Int32Visualizer:\n"
      "  id: \"Int 32 visualizer\"\n"
      "  type: consumer\n"
      "  strategy: lifo\n"
      "  parentId: \"Int 32 generator\"\n"
      "\n"
      "Int32ToDoubleConverter:\n"
      "  id: \"Int 32 to double\"\n"
      "  type: producerConsumer\n"
      "  strategy: lifo\n"
      "  parentId: \"Int 32 generator\"\n"
      "\n"
      "DoubleVisualizer:\n"
      "  id: \"Double visualizer\"\n"
      "  type: consumer\n"
      "  strategy: lifo\n"
      "  parentId: \"Int 32 to double\"\n";

  static constexpr auto PipelineFilePath = "pipeline.yaml";

 private:
  static ofstream PipelineFile;
};

decltype(YamlToPipeline_test::PipelineFile) YamlToPipeline_test::PipelineFile;

TEST_F(YamlToPipeline_test, YamlToPipeline_fromStringWorks) {
  auto pipeline = YamlToPipeline::parseFromString(PipelineStr);
  const auto& stages = pipeline->getStages();

  ASSERT_EQ(stages.size(), 4);
  EXPECT_EQ(stages[0]->getName(), "Int32RandomGenerator");
  EXPECT_EQ(stages[0]->getId(), "Int 32 generator");
  EXPECT_EQ(stages[0]->getParentId(), nullopt);

  EXPECT_EQ(stages[1]->getName(), "Int32Visualizer");
  EXPECT_EQ(stages[1]->getId(), "Int 32 visualizer");
  EXPECT_EQ(stages[1]->getParentId(), "Int 32 generator");

  EXPECT_EQ(stages[2]->getName(), "Int32ToDoubleConverter");
  EXPECT_EQ(stages[2]->getId(), "Int 32 to double");
  EXPECT_EQ(stages[2]->getParentId(), "Int 32 generator");

  EXPECT_EQ(stages[3]->getName(), "DoubleVisualizer");
  EXPECT_EQ(stages[3]->getId(), "Double visualizer");
  EXPECT_EQ(stages[3]->getParentId(), "Int 32 to double");
}

TEST_F(YamlToPipeline_test, YamlToPipeline_fromFileWorks) {
  auto pipeline = YamlToPipeline::parseFromFile(PipelineFilePath);
  const auto& stages = pipeline->getStages();

  ASSERT_EQ(stages.size(), 4);
  EXPECT_EQ(stages[0]->getName(), "Int32RandomGenerator");
  EXPECT_EQ(stages[0]->getId(), "Int 32 generator");
  EXPECT_EQ(stages[0]->getParentId(), nullopt);

  EXPECT_EQ(stages[1]->getName(), "Int32Visualizer");
  EXPECT_EQ(stages[1]->getId(), "Int 32 visualizer");
  EXPECT_EQ(stages[1]->getParentId(), "Int 32 generator");

  EXPECT_EQ(stages[2]->getName(), "Int32ToDoubleConverter");
  EXPECT_EQ(stages[2]->getId(), "Int 32 to double");
  EXPECT_EQ(stages[2]->getParentId(), "Int 32 generator");

  EXPECT_EQ(stages[3]->getName(), "DoubleVisualizer");
  EXPECT_EQ(stages[3]->getId(), "Double visualizer");
  EXPECT_EQ(stages[3]->getParentId(), "Int 32 to double");
}

TEST_F(YamlToPipeline_test, YamlToPipeline_fromStrinRunnable) {
  auto pipeline = YamlToPipeline::parseFromString(PipelineStr);
  pipeline->run();
  SteadyClock::waitForMs(100);
  pipeline->shutdown();
}

TEST_F(YamlToPipeline_test, YamlToPipeline_fromFileRunnable) {
  auto pipeline = YamlToPipeline::parseFromFile(PipelineFilePath);
  pipeline->run();
  SteadyClock::waitForMs(100);
  pipeline->shutdown();
}
