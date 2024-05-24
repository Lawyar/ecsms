#pragma once

#include "ConsumerStage.h"

class Int32Visualizer : public ConsumerStage<int32_t> {
public:
  Int32Visualizer(const std::string_view stageName, TaskRetrieveStrategy,
                  std::shared_ptr<InStageConnection<int32_t>>);

  void consume(std::shared_ptr<int32_t> inData) override;
};
