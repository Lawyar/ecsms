#include "Pipeline.h"

#include <stdexcept>

using namespace std;

Pipeline::~Pipeline() {
  shutdown();
}

void Pipeline::addStage(std::shared_ptr<IPipelineStage> stage) {
  if (!stage)
    throw std::invalid_argument("stage is null");

  m_stages.push_back(stage);
}

void Pipeline::addConnection(std::shared_ptr<StageConnection> connection) {
  if (!connection)
    throw std::invalid_argument("connection is null");

  m_connections.push_back(connection);
}

const vector<std::shared_ptr<IPipelineStage>>& Pipeline::getStages() {
  return m_stages;
}

const std::vector<std::shared_ptr<StageConnection>>&
Pipeline::getConnections() {
  return m_connections;
}

std::shared_ptr<IPipelineStage> Pipeline::getStage(
    const std::string_view stageName) {
  for (auto& stage : m_stages) {
    if (stage->getName() == stageName)
      return stage;
  }

  throw std::invalid_argument(std::string("stage ") + stageName.data() +
                              " not found");
}

void Pipeline::run() {
  for (auto& it : m_stages) 
    it->run();
}

void Pipeline::shutdown() {
  for (auto& it : m_connections)
    it->shutdown();

  for (auto& it : m_stages)
    it->shutdown();
}

Pipeline::Pipeline() {}
