#pragma once

#include "PipelineStageType.h"

#include <string>

template<typename TStage>
struct StageRepresentation {
  static constexpr auto stageName = TStage::stageName;
  static constexpr auto stageType = TStage::stageType;
  using elementType = typename TStage::elementType;
};
