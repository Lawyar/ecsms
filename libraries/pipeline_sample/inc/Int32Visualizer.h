#pragma once

#include "ConsumerStage.h"

class Int32Visualizer : public ConsumerStage<int32_t> {
public:
  Int32Visualizer(ConsumerStrategy strategy,
                  std::shared_ptr<InStageConnection<int32_t>>);

  void consume(std::shared_ptr<int32_t> inData) override;

public:
  static constexpr auto stageName = "Int32Visualizer";
  static constexpr auto stageType = PipelineStageType::consumer;
  using consumptionT = int32_t;
  using productionT = void;
};
