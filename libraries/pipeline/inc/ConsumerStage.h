#pragma once

#include "PipelineStage.h"

template <typename In>
class ConsumerStage : public PipelineStage<In, void> {
 public:
  ConsumerStage(const std::string_view stageName,
                ConsumptionStrategy,
                std::weak_ptr<InStageConnection<In>>);

 private:
  void consumeAndProduce(std::shared_ptr<In> data,
                         std::shared_ptr<void>) final;

  virtual void consume(std::shared_ptr<In> data) = 0;
};

template <typename In>
ConsumerStage<In>::ConsumerStage(
    const std::string_view stageName,
    ConsumptionStrategy consumerStrategy,
    std::weak_ptr<InStageConnection<In>> inConnection)
    : PipelineStage<In, void>(
          stageName,
          consumerStrategy,
          inConnection,
          std::weak_ptr<OutStageConnection<void>>()) {
  if (auto in = inConnection.lock(); in != nullptr) {
    auto consumerId = in->connectConsumer();
    setConsumerId(consumerId);
  } else
    throw PipelineException("inConnection expired");
}

template <typename In>
void ConsumerStage<In>::consumeAndProduce(std::shared_ptr<In> data,
                                          std::shared_ptr<void>) {
  consume(data);
}
