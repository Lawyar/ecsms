#pragma once

#include "ConsumerStage.h"

class TestConsumerStage : public ConsumerStage<int> {
public:
  static constexpr auto stageName = "TestConsumerStage";
  using consumptionT = int;

  TestConsumerStage(
      TaskRetrieveStrategy consumptionStrategy,
                    std::weak_ptr<InStageConnection<int>> inConnection)
      : ConsumerStage(stageName, consumptionStrategy, inConnection) {}

  void consume(std::shared_ptr<int> inData) override { releaseConsumerTask(inData); }
};
