#pragma once

#include "StageConnection.h"
#include "StageTask.h"

template <typename T>
class OutStageConnection : public virtual StageConnection {
public:
  ~OutStageConnection() override = default;

  void connectProducer();

  virtual std::shared_ptr<StageTask<T>> getProducerTask() = 0;

  virtual void releaseProducerTask(std::shared_ptr<T>, uint64_t timestamp,
                                   bool produced) = 0;

private:
  virtual void onProducerConnected() = 0;
};

template <typename T> void OutStageConnection<T>::connectProducer() {
  onProducerConnected();
}
