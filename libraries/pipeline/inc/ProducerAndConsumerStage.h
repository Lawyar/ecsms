#pragma once

#include "GenericPipelineStage.h"

template <typename In, typename Out>
class ConsumerAndProducerStage : public GenericPipelineStage<In, Out> {
public:
  ConsumerAndProducerStage(
      const std::string &stageName, ConsumerStrategy consumerStrategy,
      std::shared_ptr<InStageConnection<In>> inConnection,
      std::shared_ptr<OutStageConnection<Out>> outConnection);
};

template <typename In, typename Out>
ConsumerAndProducerStage<In, Out>::ConsumerAndProducerStage(
    const std::string &stageName, ConsumerStrategy consumerStrategy,
    std::shared_ptr<InStageConnection<In>> inConnection,
    std::shared_ptr<OutStageConnection<Out>> outConnection)
    : GenericPipelineStage<In, Out>(stageName, consumerStrategy,
                                        inConnection, outConnection) {
  if (!inConnection)
    throw std::invalid_argument("inConnection is null");

  if (!outConnection)
    throw std::invalid_argument("outConnection is null");

  auto consumerId = inConnection->connectConsumer();
  setConsumerId(consumerId);
}