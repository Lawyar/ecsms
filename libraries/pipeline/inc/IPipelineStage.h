#pragma once

#include "PipelineStageType.h"
#include "StageConnection.h"

#include <any>
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <optional>

class IPipelineStage {
 public:
  IPipelineStage(const std::string_view stageName);

  virtual ~IPipelineStage() = default;

  virtual void run() = 0;

  virtual void shutdown() = 0;

  std::string getName();

  void setId(const std::string_view);
  std::optional<std::string> getId();

  void setParentId(const std::string_view);
  std::optional<std::string> getParentId();

 protected:
  virtual void setConsumerId(size_t consumerId) = 0;

 protected:
  std::string m_stageName;

 private:
  std::optional<std::string> m_id;
  std::optional<std::string> m_parentId;
};

