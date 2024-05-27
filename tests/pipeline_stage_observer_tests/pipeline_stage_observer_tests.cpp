#include <gtest/gtest.h>

#include "ProducerStage.h"
#include "ConsumerStage.h"
#include "ProducerAndConsumerStage.h"
#include "PipelineStageFactory.h"
#include "InOutStageConnection.h"
#include "PipelineStageObserver.h"

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

  struct Parameters {
    string s;
    int i;
  };

  TestProducerStage(std::shared_ptr<OutStageConnection<int>> outConnection,
                    Parameters parameters)
      : ProducerStage(stageName, outConnection), parameters_(parameters) {}

  TestProducerStage(std::shared_ptr<OutStageConnection<int>> outConnection)
      : TestProducerStage(outConnection, {"", 0}) {}

  void produce(shared_ptr<int> outData) override {
    releaseProducerTask(outData);
  }

  Parameters parameters_;
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

TEST(PipelineRegistry_tests, PipelineRegistry_registerProducerWorks) {
  PipelineRegistry registry;
  registry.registerProducer<TestProducerStage>();
}

TEST(PipelineRegistry_tests, PipelineRegistry_constructProducerWorks) {
  PipelineRegistry registry;
  registry.registerProducer<TestProducerStage>();
  auto connection = registry.constructProducerConnection("TestProducerStage", 32);
  auto stage = registry.constructProducer("TestProducerStage", connection);
}

TEST(PipelineRegistry_tests, PipelineRegistry_registerProducerFactoryWorks) {
  PipelineRegistry registry;
  registry.registerProducerFactory<TestProducerStage>(
                           [](shared_ptr<StageConnection> connection) {
        auto outConnection = dynamic_cast<OutStageConnection<typename TestProducerStage::productionT>*>(connection.get());
    return make_shared<TestProducerStage>(
            shared_ptr<
                OutStageConnection<typename TestProducerStage::productionT>>(
            outConnection));
                           });

}
