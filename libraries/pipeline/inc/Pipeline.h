#pragma once

#include "IPipelineStage.h"
#include "StageConnection.h"

#include <vector>

class Pipeline {
 public:
  ~Pipeline();

  void run();

  void shutdown();

  void addStage(std::shared_ptr<IPipelineStage>);

  void addConnection(std::shared_ptr<StageConnection>);

  const std::vector<std::shared_ptr<IPipelineStage>>& getStages();

  const std::vector<std::shared_ptr<StageConnection>>& getConnections();

  std::shared_ptr<IPipelineStage> getStage(const std::string_view);

 private:
  std::vector<std::shared_ptr<IPipelineStage>> m_stages;
  std::vector<std::shared_ptr<StageConnection>> m_connections;
};
