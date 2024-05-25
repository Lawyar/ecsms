#pragma once

#include "PipelineStageType.h"

#include <string>

struct StageRepresentation {
  std::string stageName;
  PipelineStageType stageType;
};
