#pragma once

#include "StageConnection.h"
#include "StageTask.h"

template <typename T>
class OutStageConnection : public virtual StageConnection {
 public:
  virtual std::shared_ptr<StageTask<T>> getProducerTask() = 0;

  virtual void taskProduced(std::shared_ptr<T> taskData,
                                   size_t newTaskId,
                                   bool produced = true) = 0;
};
