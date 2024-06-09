#pragma once

#include "PipelineStageType.h"
#include "ConsumptionStrategy.h"

#include <string_view>

namespace PipelineHelpers {
PipelineStageType stagetTypeFromString(const std::string_view);
std::string toString(PipelineStageType);

ConsumptionStrategy strategyFromString(const std::string_view);
std::string toString(ConsumptionStrategy);
}  // namespace PipelineHelpers
