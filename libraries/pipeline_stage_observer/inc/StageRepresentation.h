#pragma once

#include "PipelineStageType.h"

#include <string>

template<typename T>
struct StageRepresentation {
  std::string stageName;
  PipelineStageType stageType;
};
