#include "Pipeline.h"

#include <stdexcept>

using namespace std;

Pipeline::~Pipeline() {
  shutdown();
}

void Pipeline::add_stage(std::shared_ptr<PipelineStage> stage) {
  if (!stage)
    throw std::invalid_argument("stage is null");

  _stages.push_back(stage);
}

void Pipeline::add_connection(std::shared_ptr<StageConnection> connection) {
  if (!connection)
    throw std::invalid_argument("connection is null");

  _connections.push_back(connection);
}

vector<std::shared_ptr<PipelineStage>> &Pipeline::getStages() { return _stages; }

std::shared_ptr<PipelineStage>
Pipeline::getStage(const std::string &stageName) {
  for (auto &stage : _stages) {
    if (stage->getName() == stageName)
      return stage;
  }

  throw std::invalid_argument(std::string("stage ") + stageName +
                              " not found");
}

void Pipeline::run() {
  for (auto &it : _stages)
    it->run();
}

void Pipeline::shutdown() {
  for (auto &it : _connections)
    it->shutdown();

  for (auto &it : _stages)
    it->stop();
}
