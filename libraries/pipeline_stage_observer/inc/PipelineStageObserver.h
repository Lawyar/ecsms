#pragma once

#include "PipelineStageType.h"
#include "StageRepresentation.h"

#include <memory>
#include <vector>

class PipelineStageObserver {
public:
  static void init();

  static std::vector<StageRepresentation> getStages();

  static bool connectable(const StageRepresentation& first,
                          const StageRepresentation& second);

private:
  static void addStage(const std::string_view stageName,
                       PipelineStageType stageType);

  PipelineStageObserver();

private:
  static std::unique_ptr<PipelineStageObserver> m_singleton;
  static bool m_initialized;

  std::vector<StageRepresentation> m_stages;
};
