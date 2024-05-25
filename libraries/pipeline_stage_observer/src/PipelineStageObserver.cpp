#include "PipelineStageObserver.h"
#include "PipelineStageObserverException.h"
#include "Int32RandomGenerator.h"
#include "Int32Visualizer.h"

#include <algorithm>

using namespace std;
using namespace std::string_literals;

decltype(PipelineStageObserver::m_singleton)
    PipelineStageObserver::m_singleton =
        unique_ptr<PipelineStageObserver>(new PipelineStageObserver());

decltype(PipelineStageObserver::m_initialized)
    PipelineStageObserver::m_initialized = false;

void PipelineStageObserver::init() {
  if (m_initialized)
    throw PipelineStageObserverException(
        "PipelineStageObserver has already been initialized");

  addStage(Int32RandomGenerator::stageName, Int32RandomGenerator::stageType);
  addStage(Int32Visualizer::stageName, Int32Visualizer::stageType);

  m_initialized = true;
}

void PipelineStageObserver::addStage(const std::string_view stageName,
                                     PipelineStageType stageType) {
  StageRepresentation r{string(stageName), stageType};

  auto &stages = m_singleton->m_stages;
  auto duplicateNameFound = std::find_if(stages.begin(), stages.end(),
               [&r](const StageRepresentation &entry) {
                 return entry.stageName == r.stageName;
               });

  if (duplicateNameFound != stages.end())
    throw PipelineStageObserverException(
        "stage with name "s + r.stageName +
        " had been already added in PipelineStageObserver");

  stages.push_back(std::move(r));
}

std::vector<StageRepresentation> PipelineStageObserver::getStages() {
  if (!m_initialized)
    throw PipelineStageObserverException(
        "PipelineStageObserver is not initialized");

  return m_singleton->m_stages;
}

PipelineStageObserver::PipelineStageObserver() {}
