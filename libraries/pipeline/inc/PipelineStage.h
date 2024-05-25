#pragma once

#include "StageConnection.h"
#include "PipelineStageType.h"

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
  virtual std::shared_ptr<StageConnection> getInConnection() = 0;

protected:
  std::string m_stageName;

  std::atomic_bool m_shutdownSignaled;
  std::thread m_thread;
};
