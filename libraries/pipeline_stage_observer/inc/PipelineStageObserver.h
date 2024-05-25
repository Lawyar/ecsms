#pragma once

#include "StageRepresentation.h"

#include <memory>
#include <vector>

class PipelineStageObserver {
public:
  template <typename T>
  static void addStage();

  static std::vector<StageRepresentation> getStages();

  static bool connectable(const StageRepresentation& first,
                          const StageRepresentation& second);

private:
  PipelineStageObserver();

private:
  static std::unique_ptr<PipelineStageObserver> m_singleton;

  std::vector<StageRepresentation> m_stages;
};
