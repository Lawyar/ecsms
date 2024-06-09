#pragma once

#include "ConsumptionStrategy.h"
#include "PipelineStageType.h"

#include <optional>
#include <string>

struct YamlPipelineStage {
  std::string stageName;
  std::string stageId;
  PipelineStageType stageType;
  std::optional<std::string> parentStageId;
  std::optional<ConsumptionStrategy> consumptionStrategy;
};
