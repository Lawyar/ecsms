#pragma once

#include "ConnectablePipelineStage.h"

template <typename In>
class ConsumerStage : public ConnectablePipelineStage<In, void> {
public:
  ConsumerStage(const std::string_view stageName, TaskRetrieveStrategy,
                std::weak_ptr<InStageConnection<In>>);

private:
  virtual void consume(std::shared_ptr<In> inData) = 0;

  void consumeAndProduce(std::shared_ptr<In> inData, std::shared_ptr<void>) override;

  void releaseProducerTask(std::shared_ptr<void>, bool) = delete;
};

template <typename In>
ConsumerStage<In>::ConsumerStage(
    const std::string_view stageName, TaskRetrieveStrategy consumerStrategy,
    std::weak_ptr<InStageConnection<In>> inConnection)
    : ConnectablePipelineStage<In, void>(stageName, consumerStrategy, inConnection,
          std::weak_ptr<OutStageConnection<void>>()) {
  if (auto in = inConnection.lock(); in != nullptr) {
    auto consumerId = in->connectConsumer();
    setConsumerId(consumerId);
  } else
    throw PipelineException("inConnection expired");
}

template <typename In>
void ConsumerStage<In>::consumeAndProduce(std::shared_ptr<In> inData,
                                 std::shared_ptr<void>) {
  consume(inData);
}
