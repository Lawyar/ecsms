#pragma once

#include "StageConnection.h"
#include "PipelineStageType.h"

#include <any>
#include <atomic>
#include <memory>
#include <string>
#include <thread>

class PipelineStage {
public:
  PipelineStage(const std::string_view stageName);
  virtual ~PipelineStage();

  virtual void run() = 0;
  void stop();

  std::string getName();

  virtual PipelineStageType getStageType() = 0;
  virtual bool consumerTasksAvailable() = 0;

  template<typename T, typename... TArgs>
  void setStageParameters(TArgs&&... );

protected:
  virtual void set(std::any);

protected:
  std::string m_stageName;

  std::atomic_bool m_shutdownSignaled;
  std::thread m_thread;
};

template <typename T, typename... TArgs>
void PipelineStage::setStageParameters(TArgs &&...args) {
  set(T{std::forward<Args>(args)...});
}
