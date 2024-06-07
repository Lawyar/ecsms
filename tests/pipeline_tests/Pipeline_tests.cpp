#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Pipeline.h"
#include "ConsumerStage.h"
#include "ProducerStage.h"
#include "InOutStageConnection.h"

using namespace std;
using namespace testing;

class TestConsumerStage : public ConsumerStage<int> {
 public:
  static constexpr auto stageName = "TestConsumerStage";
  using consumptionT = int;

  TestConsumerStage(ConsumerStrategy consumptionStrategy,
                    std::weak_ptr<InStageConnection<int>> inConnection)
      : ConsumerStage(stageName, consumptionStrategy, inConnection) {}

  MOCK_METHOD(void, consume, (shared_ptr<int> inData), (override));

  void consumeImpl(shared_ptr<int> outData) {
    releaseConsumptionData(outData);
  }
};

class TestProducerStage : public ProducerStage<int> {
 public:
  static constexpr auto stageName = "TestProducerStage";
  using productionT = int;

  TestProducerStage(std::weak_ptr<OutStageConnection<int>> outConnection)
      : ProducerStage(stageName, outConnection) {}

  MOCK_METHOD(void, produce, (shared_ptr<int> outData), (override));

  void produceImpl(shared_ptr<int> outData, int outValue, bool produced) {
    *outData = outValue;
    releaseProductionData(outData, produced);
  }
};

TEST(Pipeline_tests, getStagesWorksOnEmpty) {
  Pipeline p;
  vector<shared_ptr<IPipelineStage>> stages;

  ASSERT_NO_THROW(stages = p.getStages());
  ASSERT_TRUE(stages.empty());
}

TEST(Pipeline_tests, addStageWorks) {
  Pipeline p;

  auto connection = make_shared<
      InOutStageConnection<typename TestConsumerStage::consumptionT>>(32);
  auto stage = make_shared<TestConsumerStage>(ConsumerStrategy::consumeNewest, connection);

  p.addConnection(connection);
  p.addStage(stage);

  auto stages = p.getStages();
  ASSERT_EQ(stages.size(), 1);

  auto stageFromPipeline = stages.front();
  ASSERT_EQ(stageFromPipeline, stage);
}

TEST(Pipeline_tests, getStageWorks) {
  Pipeline p;

  auto connection = make_shared<
      InOutStageConnection<typename TestConsumerStage::consumptionT>>(32);
  auto stage =
      make_shared<TestConsumerStage>(ConsumerStrategy::consumeNewest, connection);

  p.addConnection(connection);
  p.addStage(stage);

  shared_ptr<IPipelineStage> stageFromPipeline;
  ASSERT_NO_THROW(stageFromPipeline = p.getStage(TestConsumerStage::stageName));
  ASSERT_EQ(stageFromPipeline, stage);
}

TEST(Pipeline_tests, getStageThrowsOnUnexisting) {
  Pipeline p;

  auto connection = make_shared<
      InOutStageConnection<typename TestConsumerStage::consumptionT>>(32);
  auto stage =
      make_shared<TestConsumerStage>(ConsumerStrategy::consumeNewest, connection);

  p.addConnection(connection);
  p.addStage(stage);

  ASSERT_THROW(p.getStage(string(TestConsumerStage::stageName) + "abcd"), invalid_argument);
}

TEST(Pipeline_tests, singleConsumerConsumes) {
  Pipeline p;

  auto connection = make_shared<
      InOutStageConnection<typename TestConsumerStage::consumptionT>>(32);
  auto producer = make_shared<TestProducerStage>(connection);
  auto consumer =
      make_shared<TestConsumerStage>(ConsumerStrategy::consumeOldest, connection);

  p.addConnection(connection);
  p.addStage(producer);
  p.addStage(consumer);

  atomic<int> productionValue = 0;

  vector<int> producedValues;
  vector<int> consumedValues;
  EXPECT_CALL(*producer, produce(_)).WillRepeatedly([&](shared_ptr<int> out) {
    int value = productionValue++;
    producer->produceImpl(out, value, true);
    producedValues.push_back(value);
  });
  EXPECT_CALL(*consumer, consume(_)).WillRepeatedly([&](shared_ptr<int> in) {
    consumer->consumeImpl(in);
    consumedValues.push_back(*in);
  });

  p.run();
  SteadyClock::waitForMs(100);
  p.shutdown();

  auto consumedCount = consumedValues.size();
  for (size_t i = 0; i < consumedCount; ++i)
    ASSERT_EQ(producedValues[i], consumedValues[i]);
}

TEST(Pipeline_tests, multipleConsumersConsume) {
  Pipeline p;

  auto connection = make_shared<
      InOutStageConnection<typename TestConsumerStage::consumptionT>>(32);
  auto producer = make_shared<TestProducerStage>(connection);
  auto consumer1 =
      make_shared<TestConsumerStage>(ConsumerStrategy::consumeOldest, connection);
  auto consumer2 =
      make_shared<TestConsumerStage>(ConsumerStrategy::consumeOldest, connection);
  auto consumer3 =
      make_shared<TestConsumerStage>(ConsumerStrategy::consumeOldest, connection);
  auto consumer4 =
      make_shared<TestConsumerStage>(ConsumerStrategy::consumeOldest, connection);

  p.addConnection(connection);
  p.addStage(producer);
  p.addStage(consumer1);
  p.addStage(consumer2);
  p.addStage(consumer3);
  p.addStage(consumer4);

  atomic<int> productionValue = 0;

  vector<int> producedValues;
  vector<int> consumedValues_1;
  vector<int> consumedValues_2;
  vector<int> consumedValues_3;
  vector<int> consumedValues_4;
  EXPECT_CALL(*producer, produce(_)).WillRepeatedly([&](shared_ptr<int> out) {
    int value = productionValue++;
    SteadyClock::waitForMs(10);
    producer->produceImpl(out, value, true);
    producedValues.push_back(value);
  });
  EXPECT_CALL(*consumer1, consume(_)).WillRepeatedly([&](shared_ptr<int> in) {
    consumer1->consumeImpl(in);
    consumedValues_1.push_back(*in);
  });
  EXPECT_CALL(*consumer2, consume(_)).WillRepeatedly([&](shared_ptr<int> in) {
    consumer2->consumeImpl(in);
    consumedValues_2.push_back(*in);
  });
  EXPECT_CALL(*consumer3, consume(_)).WillRepeatedly([&](shared_ptr<int> in) {
    consumer3->consumeImpl(in);
    consumedValues_3.push_back(*in);
  });
  EXPECT_CALL(*consumer4, consume(_)).WillRepeatedly([&](shared_ptr<int> in) {
    consumer4->consumeImpl(in);
    consumedValues_4.push_back(*in);
  });

  p.run();
  SteadyClock::waitForMs(100);
  p.shutdown();

  for (size_t i = 0; i < consumedValues_1.size(); ++i)
    ASSERT_EQ(consumedValues_1[i], producedValues[i]);
  for (size_t i = 0; i < consumedValues_2.size(); ++i)
    ASSERT_EQ(consumedValues_2[i], producedValues[i]);
  for (size_t i = 0; i < consumedValues_3.size(); ++i)
    ASSERT_EQ(consumedValues_3[i], producedValues[i]);
  for (size_t i = 0; i < consumedValues_4.size(); ++i)
    ASSERT_EQ(consumedValues_4[i], producedValues[i]);
}
