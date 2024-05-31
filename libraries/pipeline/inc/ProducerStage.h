#pragma once

#include "ConnectablePipelineStage.h"

template <typename Out>
class ProducerStage : public ConnectablePipelineStage<void, Out> {
public:
  ProducerStage(const std::string_view stageName,
                std::weak_ptr<OutStageConnection<Out>> outConnection);

private:
  virtual void produce(std::shared_ptr<Out> outData) = 0;

  void consumeAndProduce(std::shared_ptr<void>, std::shared_ptr<Out> outData) override;

  void releaseConsumerTask(std::shared_ptr<void>) = delete;
};

template <typename Out>
ProducerStage<Out>::ProducerStage(
    const std::string_view stageName,
    std::weak_ptr<OutStageConnection<Out>> outConnection)
    : ConnectablePipelineStage<void, Out>(stageName, nullopt, std::weak_ptr<InStageConnection<void>>(),
                                          outConnection) {
  if (auto out = outConnection.lock(); out != nullptr)
    out->connectProducer();
  else
    throw PipelineException("outConnection expired");
}

template <typename Out>
void ProducerStage<Out>::consumeAndProduce(std::shared_ptr<void>,
                                  std::shared_ptr<Out> outData) {
  produce(outData);
}
