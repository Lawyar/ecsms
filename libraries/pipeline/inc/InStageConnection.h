#pragma once

#include "StageConnection.h"
#include "StageTask.h"
#include "TaskRetrieveStrategy.h"
#include "PipelineException.h"

#include <algorithm>
#include <vector>

template <typename T>
class InStageConnection : public virtual StageConnection {
public:
  InStageConnection();

  size_t connectConsumer();

  virtual std::shared_ptr<StageTask<T>>
  getConsumerTask(size_t consumerId, TaskRetrieveStrategy strategy,
                  uint64_t least_timestamp) = 0;
  virtual void releaseConsumerTask(std::shared_ptr<T>, size_t consumer_id) = 0;

  virtual bool consumerTasksAvailable(size_t consumer_id,
                                      uint64_t least_timestamp) const = 0;

private:
  virtual size_t onConsumerConnected() = 0;

public:
  static constexpr size_t max_consumers_count = 4u;
};

template <typename T> InStageConnection<T>::InStageConnection() {}

template <typename T> size_t InStageConnection<T>::connectConsumer() {
  auto consumer_id = onConsumerConnected();

  if (consumer_id >= max_consumers_count)
    throw PipelineException("Cannot connect consumer: the consumers' limit has been reached");

  return consumer_id;
}
