#pragma once

#include "ProducerAndConsumerStage.h"

#include <cstdint>

class Int32ToDoubleConverter
    : public ConsumerAndProducerStage<int32_t, double> {
 public:
  Int32ToDoubleConverter(
      ConsumerStrategy strategy,
      std::shared_ptr<InStageConnection<int32_t>> inConnection,
      std::shared_ptr<OutStageConnection<double>> outConnection);

  void consumeAndProduce(std::shared_ptr<int32_t> inData,
                         std::shared_ptr<double> outData) override;

  static constexpr auto stageName = "Int32ToDoubleConverter";
  static constexpr auto stageType = PipelineStageType::producerConsumer;
  using consumptionT = int32_t;
  using productionT = double;
};
