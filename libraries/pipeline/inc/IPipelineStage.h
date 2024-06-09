#pragma once

#include "PipelineStageType.h"
#include "StageConnection.h"
#include "ConsumptionStrategy.h"

#include <any>
#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <thread>

class IPipelineStage {
 public:
  IPipelineStage(const std::string_view stageName);

  virtual ~IPipelineStage() = default;

  virtual void run() = 0;

  virtual void shutdown() = 0;

  std::string getName() const;

  void setId(const std::string_view id);
  std::optional<std::string> getId() const;

  void setParentId(const std::string_view parentId);
  std::optional<std::string> getParentId() const;

  virtual PipelineStageType getStageType() const = 0;

  virtual std::optional<ConsumptionStrategy> getConsumptionStrategy() const = 0;

 protected:
  std::string m_stageName;

 private:
  std::optional<std::string> m_id;
  std::optional<std::string> m_parentId;
};
