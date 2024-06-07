#pragma once

#include "PipelineStageType.h"
#include "StageConnection.h"

#include <any>
#include <atomic>
#include <memory>
#include <string>
#include <thread>

class IPipelineStage {
 public:
  IPipelineStage(const std::string_view stageName);

  virtual ~IPipelineStage() = default;

  virtual void run() = 0;

  virtual void shutdown() = 0;

  std::string getName();

  template <typename T, typename... TArgs>
  void setStageParameters(TArgs&&...);

 protected:
  virtual void set(std::any);

  virtual void setConsumerId(size_t consumerId) = 0;

 protected:
  std::string m_stageName;
};

template <typename T, typename... TArgs>
void IPipelineStage::setStageParameters(TArgs&&... args) {
  set(T{std::forward<Args>(args)...});
}
