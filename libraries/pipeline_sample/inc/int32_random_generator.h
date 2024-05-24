#pragma once

#include "ProducerStage.h"

class Int32RandomGenerator : public ProducerStage<int32_t> {
public:
  Int32RandomGenerator(
      const std::string_view stageName,
      std::shared_ptr<OutStageConnection<int32_t>> outConnection);

  void produce(std::shared_ptr<int32_t> outData) override;
};
