#pragma once

#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineStageType.h"

#include <memory>

template<typename StageT> class PipelineStageFactory {
public:
  template<typename... Args>
  std::shared_ptr<StageT>
  create(TaskRetrieveStrategy consumptionStrategy,
         std::shared_ptr<InStageConnection<typename StageT::consumptionT>>
             connection,
         Args... args) {
    static_assert(StageT::stageType == PipelineStageType::consumer ||
                  StageT::stageType == PipelineStageType::producerConsumer);
    return std::make_shared<StageT>(consumptionStrategy, connection, std::forward<Args>(args)...);
  }

  template <typename... Args>
  std::shared_ptr<StageT>
  create(std::shared_ptr<OutStageConnection<typename StageT::productionT>>
             connection,
         Args... args) {
    static_assert(StageT::stageType == PipelineStageType::producer ||
                  StageT::stageType == PipelineStageType::producerConsumer);
    return std::make_shared<StageT>(connection, std::forward<Args>(args)...);
  }

  template <typename... Args>
  std::shared_ptr<StageT>
  create(TaskRetrieveStrategy consumptionStrategy,
         std::shared_ptr<InStageConnection<typename StageT::consumptionT>>
             inConnection,
         std::shared_ptr<OutStageConnection<typename StageT::productionT>>
             outConnection,
         Args... args) {
    static_assert(std::is_same_v<typename StageT::consumptionT,
                                 typename StageT::productionT>);
    static_assert(StageT::stageType == PipelineStageType::producerConsumer);
    return std::make_shared<StageT>(consumptionStrategy, inConnection, outConnection,
                               std::forward<Args>(args)...);
  }
};
