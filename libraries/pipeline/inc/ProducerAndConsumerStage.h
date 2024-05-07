#pragma once

#include "ConnectablePipelineStage.h"

template <typename In, typename Out>
class ProducerAndConsumerStage : public ConnectablePipelineStage<In, Out> {
public:
  ProducerAndConsumerStage(
      const std::string &stageName, TaskRetrieveStrategy consumerStrategy,
      std::shared_ptr<InStageConnection<In>> inConnection,
      std::shared_ptr<OutStageConnection<Out>> outConnection);
};

template <typename In, typename Out>
ProducerAndConsumerStage<In, Out>::ProducerAndConsumerStage(
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

