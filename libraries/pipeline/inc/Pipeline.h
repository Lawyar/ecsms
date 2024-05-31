#pragma once

#include "PipelineStage.h"
#include "StageConnection.h"

#include <vector>

class Pipeline {
public:
  virtual ~Pipeline();

  void addStage(std::shared_ptr<PipelineStage>);
  void addConnection(std::shared_ptr<StageConnection>);

  std::vector<std::shared_ptr<PipelineStage>> getStages();
  std::shared_ptr<PipelineStage> getStage(const std::string &);

  void run();
  void shutdown();

private:
  std::vector<std::shared_ptr<PipelineStage>> m_stages;
  std::vector<std::shared_ptr<StageConnection>> m_connections;
};
