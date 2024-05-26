#include <gtest/gtest.h>

#include "ProducerStage.h"
#include "ConsumerStage.h"
#include "ProducerAndConsumerStage.h"
#include "PipelineStageFactory.h"
#include "InOutStageConnection.h"

using namespace std;

class TestConsumerStage : public ConsumerStage<int> {
public:
  static constexpr auto stageName = "TestConsumerStage";
  static constexpr auto stageType = PipelineStageType::consumer;
  using consumptionT = int;
  using productionT = void;

  TestConsumerStage(
      TaskRetrieveStrategy consumptionStrategy,
                    std::shared_ptr<InStageConnection<int>> inConnection, string s, int i)
      : ConsumerStage(stageName, consumptionStrategy, inConnection), additionalField_1(move(s)), additionalField_2(i) {}

    TestConsumerStage(TaskRetrieveStrategy consumptionStrategy,
                    std::shared_ptr<InStageConnection<int>> inConnection)
      : TestConsumerStage(consumptionStrategy, inConnection, "", 0) {}

  void consume(shared_ptr<int> inData) override { releaseConsumerTask(inData); }

  string additionalField_1;
  int additionalField_2;
};

class TestProducerStage : public ProducerStage<int> {
public:
  static constexpr auto stageName = "TestProducerStage";
  static constexpr auto stageType = PipelineStageType::producer;
  using consumptionT = void;
  using productionT = int;

  TestProducerStage(std::shared_ptr<OutStageConnection<int>> outConnection,
                    string s, int i)
      : ProducerStage(stageName, outConnection), additionalField_1(move(s)),
        additionalField_2(i) {}

  TestProducerStage(std::shared_ptr<OutStageConnection<int>> outConnection)
      : TestProducerStage(outConnection, "", 0) {}

  void produce(shared_ptr<int> outData) override {
    releaseProducerTask(outData);
  }

  string additionalField_1;
  int additionalField_2;
};

class TestConsumerAndProducerStage : public ConsumerAndProducerStage<int, int> {
public:
  static constexpr auto stageName = "TestConsumerAndProducerStage";
  static constexpr auto stageType = PipelineStageType::producerConsumer;
  using consumptionT = int;
  using productionT = int;

    TestConsumerAndProducerStage(
    TaskRetrieveStrategy consumptionStrategy,
    std::shared_ptr<InStageConnection<int>> inConnection,
    std::shared_ptr<OutStageConnection<int>> outConnection, string s, int i)
    : ConsumerAndProducerStage(stageName, consumptionStrategy, inConnection,
                                outConnection),
    additionalField_1(s), additionalField_2(i) {}

  TestConsumerAndProducerStage(
      TaskRetrieveStrategy consumptionStrategy,
      std::shared_ptr<InStageConnection<int>> inConnection,
      std::shared_ptr<OutStageConnection<int>> outConnection)
        : TestConsumerAndProducerStage(consumptionStrategy,
                                       inConnection,
                                       outConnection, "", 0) {}

  void consumeAndProduce(shared_ptr<int> inData,
                         shared_ptr<int> outData) override {
    releaseConsumerTask(inData);
    releaseProducerTask(outData);
  }

  string additionalField_1;
  int additionalField_2;
};

TEST(pipeline_stage_observer_tests,
     PipelineStageFactory_createConsumerStageWorks) {
  auto connection = make_shared<InOutStageConnection<int>>(32);
  shared_ptr<InStageConnection<int>> in = connection;

  PipelineStageFactory<TestConsumerStage> factory;
  shared_ptr<TestConsumerStage> stage;
  ASSERT_NO_THROW(stage =
                      factory.create(TaskRetrieveStrategy::newest, in));
  ASSERT_NE(stage, nullptr);
}

TEST(pipeline_stage_observer_tests,
     PipelineStageFactory_createConsumerStageWithAdditionalArgsWorks) {
  auto connection = make_shared<InOutStageConnection<int>>(32);
  shared_ptr<InStageConnection<int>> in = connection;

  PipelineStageFactory<TestConsumerStage> factory;
  shared_ptr<TestConsumerStage> stage;
  string s = "64";
  int i = 128;
  ASSERT_NO_THROW(stage = factory.create(TaskRetrieveStrategy::newest, in, s, i));
  ASSERT_EQ(stage->additionalField_1, s);
  ASSERT_EQ(stage->additionalField_2, i);
}

TEST(pipeline_stage_observer_tests,
     PipelineStageFactory_createProducerStageWorks) {
  auto connection = make_shared<InOutStageConnection<int>>(32);
  shared_ptr<OutStageConnection<int>> out = connection;

  PipelineStageFactory<TestProducerStage> factory;
  shared_ptr<TestProducerStage> stage;
  ASSERT_NO_THROW(stage = factory.create(out));
  ASSERT_NE(stage, nullptr);
}

TEST(pipeline_stage_observer_tests,
    PipelineStageFactory_createProducerStageWorksWithAdditionalArgs) {
  auto connection = make_shared<InOutStageConnection<int>>(32);
  shared_ptr<OutStageConnection<int>> out = connection;

  PipelineStageFactory<TestProducerStage> factory;
  shared_ptr<TestProducerStage> stage;
  string s = "64";
  int i = 128;
  ASSERT_NO_THROW(stage = factory.create(out, s, i));
  ASSERT_EQ(stage->additionalField_1, s);
  ASSERT_EQ(stage->additionalField_2, i);
}

TEST(pipeline_stage_observer_tests, PipelineStageFactory_createConsumerAndProducerStageWorks) {
  auto connection = make_shared<InOutStageConnection<int>>(32);
  shared_ptr<InStageConnection<int>> in = connection;
  shared_ptr<OutStageConnection<int>> out = connection;

  PipelineStageFactory<TestConsumerAndProducerStage> factory;
  shared_ptr<TestConsumerAndProducerStage> stage;
  ASSERT_NO_THROW(stage = factory.create(TaskRetrieveStrategy::newest, in, out));
  ASSERT_NE(stage, nullptr);
}

TEST(pipeline_stage_observer_tests,
    PipelineStageFactory_createConsumerAndProducerStageWithAdditionalArgsWorks) {
  auto connection = make_shared<InOutStageConnection<int>>(32);
  shared_ptr<InStageConnection<int>> in = connection;
  shared_ptr<OutStageConnection<int>> out = connection;

  PipelineStageFactory<TestConsumerAndProducerStage> factory;
  shared_ptr<TestConsumerAndProducerStage> stage;
  string s = "64";
  int i = 128;
  ASSERT_NO_THROW(stage =
                      factory.create(TaskRetrieveStrategy::newest, in, out, s, i));
  ASSERT_EQ(stage->additionalField_1, s);
  ASSERT_EQ(stage->additionalField_2, i);
}
