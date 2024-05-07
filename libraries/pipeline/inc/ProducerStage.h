#pragma once

#include "ConnectablePipelineStage.h"

template <typename Out>
class ProducerStage : public ConnectablePipelineStage<void, Out> {
public:
  ProducerStage(const std::string &stageName,
                std::shared_ptr<OutStageConnection<Out>> outConnection);

private:
  virtual void produce(std::shared_ptr<Out> out_data) = 0;

  void function(std::shared_ptr<void>,
                std::shared_ptr<Out> out_data) override;

  void releaseConsumerTask(std::shared_ptr<void>) = delete;
};

template <typename Out>
ProducerStage<Out>::ProducerStage(const std::string &stageName,
            std::shared_ptr<OutStageConnection<Out>> outConnection)
    : ConnectablePipelineStage<void, Out>(stageName, nullptr, nullptr,
                                            outConnection) {
    if (!outConnection)
        throw std::invalid_argument("outConnection is null");

    outConnection->connectProducer();
}

template <typename Out>
void ProducerStage<Out>::function(std::shared_ptr<void>,
                        std::shared_ptr<Out> out_data) {
    produce(out_data);
}
