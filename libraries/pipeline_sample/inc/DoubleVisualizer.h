#pragma once

#include "ConsumerStage.h"

class DoubleVisualizer : public ConsumerStage<double> {
public:
  DoubleVisualizer(std::shared_ptr<InStageConnection<double>>);

  void consume(std::shared_ptr<double> inData) override;

public:
  static constexpr auto stageName = "DoubleVisualizer";
  static constexpr auto stageType = PipelineStageType::consumer;
  using consumptionT = double;
  using productionT = void;
};
