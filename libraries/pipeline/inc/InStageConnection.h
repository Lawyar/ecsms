#pragma once

#include "PipelineException.h"
#include "StageConnection.h"
#include "StageTask.h"
#include "TaskRetrieveStrategy.h"

#include <algorithm>
#include <vector>

template <typename T>
class InStageConnection : public virtual StageConnection {
 public:
  virtual size_t connectConsumer() = 0;

  virtual std::shared_ptr<StageTask<T>> getConsumerTask(
      size_t consumerId,
      ConsumerStrategy strategy,
      size_t minTaskId) = 0;

  virtual void releaseConsumerTask(std::shared_ptr<T> taskData,
                                   size_t consumerId) = 0;
};
