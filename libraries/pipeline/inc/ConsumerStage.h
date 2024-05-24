#pragma once

#include "ConnectablePipelineStage.h"

template <typename In>
class ConsumerStage : public ConnectablePipelineStage<In, void> {
public:
  ConsumerStage(const std::string_view stageName, TaskRetrieveStrategy,
                std::shared_ptr<InStageConnection<In>>);

private:
  virtual void consume(std::shared_ptr<In> inData) = 0;

  void function(std::shared_ptr<In> inData, std::shared_ptr<void>) override;

  void releaseProducerTask(std::shared_ptr<void>, bool) = delete;
};

template <typename In>
ConsumerStage<In>::ConsumerStage(
    const std::string_view stageName, TaskRetrieveStrategy consumerStrategy,
    std::shared_ptr<InStageConnection<In>> inConnection)
    : ConnectablePipelineStage<In, void>(stageName, consumerStrategy,
                                         inConnection, nullptr) {
  if (!inConnection)
    throw std::invalid_argument("inConnection is null");

  auto consumer_id = inConnection->connectConsumer();

  setConsumerId(consumer_id);
}

template <typename In>
void ConsumerStage<In>::function(std::shared_ptr<In> inData,
                                 std::shared_ptr<void>) {
  consume(inData);
}
