#pragma once

#include "ProducerAndConsumerStage.h"

#include <cstdint>

class Int32ToDoubleConverter
    : public ConsumerAndProducerStage<int32_t, double> {
  void function(std::shared_ptr<int32_t> inData,
                std::shared_ptr<double> outData) override;
public:
  static constexpr auto stageName = "Int32ToDoubleConverter";
  static constexpr auto stageType = PipelineStageType::producerConsumer;
  using consumptionT = int32_t;
  using productionT = double;
};
