#pragma once

#include "ConnectablePipelineStage.h"

template <typename In, typename Out>
class ConsumerAndProducerStage : public ConnectablePipelineStage<In, Out> {
public:
  ConsumerAndProducerStage(
      const std::string &stageName, TaskRetrieveStrategy consumerStrategy,
      std::shared_ptr<InStageConnection<In>> inConnection,
      std::shared_ptr<OutStageConnection<Out>> outConnection);

private:
  virtual void consumeAndProduce(std::shared_ptr<In> inData,
                                 std::shared_ptr<Out> outData) = 0;
};

template <typename In, typename Out>
ConsumerAndProducerStage<In, Out>::ConsumerAndProducerStage(
    const std::string &stageName, TaskRetrieveStrategy consumerStrategy,
    std::shared_ptr<InStageConnection<In>> inConnection,
    std::shared_ptr<OutStageConnection<Out>> outConnection)
    : ConnectablePipelineStage<In, Out>(stageName, consumerStrategy,
                                        inConnection, outConnection) {
  if (!inConnection)
    throw std::invalid_argument("inConnection is null");

  if (!outConnection)
    throw std::invalid_argument("outConnection is null");

  outConnection->connectProducer();
  auto consumer_id =
      inConnection->connect_consumer(this->getStatisticsCollector());

  setConsumerId(consumer_id);
}
