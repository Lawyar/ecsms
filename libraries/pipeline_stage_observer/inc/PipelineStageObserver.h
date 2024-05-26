#pragma once

#include "PipelineStageType.h"
#include "StageRepresentation.h"

#include <memory>
#include <vector>

class PipelineStageObserver {
public:
  static void init();

  //static std::vector<StageRepresentation> getStages();

  //static bool connectable(const StageRepresentation& first,
  //                        const StageRepresentation& second);

  
  template <typename StageT, typename... StageArgs>
  static void registerStage(StageArgs... StageArgs);

private:
  static void addStage(const std::string_view stageName,
                       PipelineStageType stageType);

  PipelineStageObserver();

  template <typename StageT, typename... StageArgs>
  static void registerProducer(StageArgs... StageArgs);

  template <typename StageT, typename... StageArgs>
  static void registerConsumer(StageArgs... StageArgs);

  template <typename StageT, typename... StageArgs>
  static void registerProducerConsumer(StageArgs... StageArgs);

private:
  static std::unique_ptr<PipelineStageObserver> m_singleton;
  static bool m_initialized;

  //std::unordered_map<std::string, std::function<>>
  //std::vector<StageRepresentation> m_stages;
};
