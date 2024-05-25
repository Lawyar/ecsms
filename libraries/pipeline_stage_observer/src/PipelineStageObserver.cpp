#include "PipelineStageObserver.h"
#include "PipelineStageObserverException.h"

using namespace std;
using namespace std::string_literals;

decltype(PipelineStageObserver::m_singleton) PipelineStageObserver::m_singleton =
    unique_ptr<PipelineStageObserver>(new PipelineStageObserver());

template <typename T>
void PipelineStageObserver::addStage() {
  StageRepresentation r<T>;

  auto &stages = m_singleton->m_stages;
  auto duplicateNameFound = std::find_if(stages.begin(), stages.end(),
               [&r](const StageRepresentation &entry) {
                 return entry.stageName == r.stageName;
               });

  if (duplicateNameFound)
    throw PipelineStageObserverException(
        "stage with name "s + r.stageName +
        " had been already added in PipelineStageObserver");

  stages.push_back(std::move(r));
}

std::vector<StageRepresentation> PipelineStageObserver::getStages() {
  return m_singleton->m_stages;
}
