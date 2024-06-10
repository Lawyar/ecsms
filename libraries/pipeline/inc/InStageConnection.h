#pragma once

#include "ConsumptionStrategy.h"
#include "PipelineException.h"
#include "StageConnection.h"
#include "StageTask.h"

#include <algorithm>
#include <vector>

template <typename T>
class InStageConnection : public virtual StageConnection {
 public:
  virtual size_t connectConsumer() = 0;

  virtual std::shared_ptr<StageTask<T>> getConsumerTask(
      size_t consumerId,
      ConsumptionStrategy strategy,
      size_t minTaskId) = 0;

  virtual void taskConsumed(std::shared_ptr<T> taskData,
                            size_t consumerId,
                            bool consumed = true) = 0;
};
