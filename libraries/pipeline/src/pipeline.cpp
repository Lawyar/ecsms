#include "Pipeline.h"

#include <stdexcept>

using namespace std;

Pipeline::~Pipeline() { shutdown(); }

void Pipeline::addStage(std::shared_ptr<PipelineStage> stage) {
  if (!stage)
    throw std::invalid_argument("stage is null");

  m_stages.push_back(stage);
}

void Pipeline::addConnection(std::shared_ptr<StageConnection> connection) {
  if (!connection)
    throw std::invalid_argument("connection is null");

  m_connections.push_back(connection);
}

vector<std::shared_ptr<PipelineStage>> &Pipeline::getStages() {
  return m_stages;
}

std::shared_ptr<PipelineStage>
Pipeline::getStage(const std::string &stageName) {
  for (auto &stage : m_stages) {
    if (stage->getName() == stageName)
      return stage;
  }

  throw std::invalid_argument(std::string("stage ") + stageName + " not found");
}

void Pipeline::run() {
  for (auto &it : m_stages)
    it->run();
}

void Pipeline::shutdown() {
  for (auto &it : m_connections)
    it->shutdown();

  for (auto &it : m_stages)
    it->stop();
}
