#include <gtest/gtest.h>

#include "ConsumerStage.h"
#include "InOutStageConnection.h"
#include "PipelineRegistry.h"
#include "ProducerAndConsumerStage.h"
#include "ProducerStage.h"

using namespace std;

class TestConsumerStage : public ConsumerStage<int> {
 public:
  static constexpr auto stageName = "TestConsumerStage";
  using consumptionT = int;

  TestConsumerStage(ConsumerStrategy consumptionStrategy,
                    std::shared_ptr<InStageConnection<int>> inConnection,
                    string s,
                    int i)
      : ConsumerStage(stageName, consumptionStrategy, inConnection),
        additionalField_1(move(s)),
        additionalField_2(i) {}

  TestConsumerStage(ConsumerStrategy consumptionStrategy,
                    std::shared_ptr<InStageConnection<int>> inConnection)
      : TestConsumerStage(consumptionStrategy, inConnection, "", 0) {}

  void consume(shared_ptr<int> inData) override { releaseConsumptionData(inData); }

  string additionalField_1;
  int additionalField_2;
};

class TestProducerStage : public ProducerStage<int> {
 public:
  static constexpr auto stageName = "TestProducerStage";
  using productionT = int;

  TestProducerStage(std::shared_ptr<OutStageConnection<int>> outConnection,
                    string s,
                    int i)
      : ProducerStage(stageName, outConnection), s(s), i(i) {}

  TestProducerStage(std::shared_ptr<OutStageConnection<int>> outConnection)
      : TestProducerStage(outConnection, "", 0) {}

  void produce(shared_ptr<int> outData) override {
    releaseProductionData(outData, true);
  }

  string s;
  int i;
};

class TestConsumerAndProducerStage : public ConsumerAndProducerStage<int, int> {
 public:
  static constexpr auto stageName = "TestConsumerAndProducerStage";
  using consumptionT = int;
  using productionT = int;

  TestConsumerAndProducerStage(
      ConsumerStrategy consumptionStrategy,
      std::shared_ptr<InStageConnection<int>> inConnection,
      std::shared_ptr<OutStageConnection<int>> outConnection,
      string s,
      int i)
      : ConsumerAndProducerStage(stageName,
                                 consumptionStrategy,
                                 inConnection,
                                 outConnection),
        additionalField_1(s),
        additionalField_2(i) {}

  TestConsumerAndProducerStage(
      ConsumerStrategy consumptionStrategy,
      std::shared_ptr<InStageConnection<int>> inConnection,
      std::shared_ptr<OutStageConnection<int>> outConnection)
      : TestConsumerAndProducerStage(consumptionStrategy,
                                     inConnection,
                                     outConnection,
                                     "",
                                     0) {}

  void consumeAndProduce(shared_ptr<int> inData,
                         shared_ptr<int> outData) override {
    releaseConsumptionData(inData);
    releaseProductionData(outData, true);
  }

  string additionalField_1;
  int additionalField_2;
};

TEST(PipelineRegistry_tests, PipelineRegistry_registerProducerWorks) {
  PipelineRegistry::Instance().registerProducer<TestProducerStage>(
      TestProducerStage::stageName);
}

TEST(PipelineRegistry_tests, PipelineRegistry_constructProducerWorks) {
  auto& registry = PipelineRegistry::Instance();
  registry.registerProducer<TestProducerStage>(
      TestProducerStage::stageName);
  auto connection =
      registry.constructProducerConnection("TestProducerStage", 32);
  auto stage = registry.constructProducer("TestProducerStage", connection);

  ASSERT_NE(connection, nullptr);
  ASSERT_NE(stage, nullptr);

  auto connectionDynamic = dynamic_pointer_cast<
      InOutStageConnection<typename TestProducerStage::productionT>>(
      connection);

  auto stageDynamic = dynamic_pointer_cast<TestProducerStage>(stage);

  ASSERT_NE(connectionDynamic, nullptr);
  ASSERT_NE(stageDynamic, nullptr);
}

TEST(PipelineRegistry_tests, PipelineRegistry_registerProducerFactoryWorks) {
  auto& registry = PipelineRegistry::Instance();

  string s = "abcdef";
  int i = 128;
  registry.registerProducerFactory<TestProducerStage>(
      TestProducerStage::stageName,
      [s, i](shared_ptr<StageConnection> connection) {
        auto outConnection = std::dynamic_pointer_cast<
            OutStageConnection<typename TestProducerStage::productionT>>(
            connection);
        return make_shared<TestProducerStage>(outConnection, s, i);
      });

  auto connection =
      registry.constructProducerConnection("TestProducerStage", 32);
  auto stage = registry.constructProducer("TestProducerStage", connection);

  auto stageDynamic = dynamic_pointer_cast<TestProducerStage>(stage);

  ASSERT_EQ(stageDynamic->s, s);
  ASSERT_EQ(stageDynamic->i, i);
}
