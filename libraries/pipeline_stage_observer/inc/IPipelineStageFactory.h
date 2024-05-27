#pragma once

#pragma once

#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineStageType.h"

#include <memory>

class IPipelineStageFactory {
public:
  template<typename StageT, typename... Args>
  virtual std::shared_ptr<StageT>
  create(TaskRetrieveStrategy consumptionStrategy,
         std::shared_ptr<InStageConnection<typename StageT::consumptionT>>
             connection,
         Args... args) = 0;

  template <typename StageT, typename... Args>
  std::shared_ptr<StageT>
  create(std::shared_ptr<OutStageConnection<typename StageT::productionT>>
             connection,
         Args... args) {
    static_assert(StageT::stageType == PipelineStageType::producer ||
                  StageT::stageType == PipelineStageType::producerConsumer);
    return std::make_shared<StageT>(connection, std::forward<Args>(args)...);
  }

  template <typename StageT, typename... Args>
  std::shared_ptr<StageT>
  create(TaskRetrieveStrategy consumptionStrategy,
         std::shared_ptr<InStageConnection<typename StageT::consumptionT>>
             inConnection,
         std::shared_ptr<OutStageConnection<typename StageT::productionT>>
             outConnection,
         Args... args);
};

