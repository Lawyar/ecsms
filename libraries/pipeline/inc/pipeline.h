#pragma once

#include "PipelineStage.h"
#include "StageConnection.h"

#include <vector>

class Pipeline {
public:
  virtual ~Pipeline();

  void add_stage(std::shared_ptr<PipelineStage>);
  void add_connection(std::shared_ptr<StageConnection>);

  std::vector<std::shared_ptr<PipelineStage>> &getStages();
  std::shared_ptr<PipelineStage> getStage(const std::string &);

  void run();
  void shutdown();

private:
  std::vector<std::shared_ptr<PipelineStage>> _stages;
  std::vector<std::shared_ptr<StageConnection>> _connections;
};
