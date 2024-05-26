#include <gtest/gtest.h>

#include "ProducerStage.h"
#include "ConsumerStage.h"
#include "ProducerAndConsumerStage.h"
#include "PipelineStageFactory.h"

class TestProducerStage : public ProducerStage<int> {
public:
  static constexpr auto stageType = PipelineStageType::consumer;
  using consumptionT = void;
  using productionT = int;

  void produce(std::shared_ptr<int> outData) override {
    releaseProducerTask(outData);
  }
};

class TestConsumerStage : public ConsumerStage<int> {
public:
  static constexpr auto stageType = PipelineStageType::producer;
  using consumptionT = int;
  using productionT = void;

  void consume(std::shared_ptr<int> inData) override {
    releaseConsumerTask(inData);
  }
};

class TestConsumerAndProducerStage : public ConsumerAndProducerStage<int, int> {
public:
  static constexpr auto stageType = PipelineStageType::producerConsumer;
  using consumptionT = int;
  using productionT = int;

  void consumeAndProduce(std::shared_ptr<int> inData,
                         std::shared_ptr<int> outData) override {
    releaseConsumerTask(inData);
    releaseProducerTask(outData);
  }
};

TEST(pipeline_stage_observer_tests, PipelineStageFactory_works) {
  PipelineStageFactory<TestConsumerAndProducerStage> factory;
  factory->create();
}
