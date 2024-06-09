#include "PipelineRegistry.h"

#include "DoubleVisualizer.h"
#include "Int32RandomGenerator.h"
#include "Int32ToDoubleConverter.h"
#include "Int32Visualizer.h"

using namespace std;

decltype(PipelineRegistry::initialized) PipelineRegistry::initialized = false;
decltype(PipelineRegistry::globalRegistry) PipelineRegistry::globalRegistry;

PipelineRegistry::PipelineRegistry() {}

void PipelineRegistry::Init() {
  initialized = true;

  globalRegistry.registerConsumer<DoubleVisualizer>(
      DoubleVisualizer::stageName);
  globalRegistry.registerProducer<Int32RandomGenerator>(
      Int32RandomGenerator::stageName);
  globalRegistry.registerConsumer<Int32Visualizer>(Int32Visualizer::stageName);
  globalRegistry.registerConsumerAndProducer<Int32ToDoubleConverter>(
      Int32ToDoubleConverter::stageName);
}

PipelineRegistry& PipelineRegistry::Instance() {
  if (!initialized)
    Init();
  return globalRegistry;
}

void PipelineRegistry::reset() {
  m_producers.clear();
  m_consumers.clear();
  m_consumersProducers.clear();
  m_producerConnections.clear();
  m_consumerConnections.clear();
  m_consumerAndProducerConnections.clear();
}

std::vector<std::string> PipelineRegistry::getStageNames() const {
  vector<string> names;

  for (const auto& it : m_producers)
    names.push_back(it.first);
  for (const auto& it : m_consumers)
    names.push_back(it.first);
  for (const auto& it : m_consumersProducers)
    names.push_back(it.first);

  return names;
}

PipelineStageType PipelineRegistry::getStageType(const std::string& key) const {
  if (m_producers.find(key) != m_producers.end())
    return PipelineStageType::producer;
  else if (m_consumers.find(key) != m_consumers.end())
    return PipelineStageType::consumer;
  else if (m_consumersProducers.find(key) != m_consumersProducers.end()) {
    return PipelineStageType::producerConsumer;
  }

  throw PipelineRegistryException(std::string("key ") + key +
                                  " was not presented in registry");
}

std::shared_ptr<StageConnection> PipelineRegistry::constructProducerConnection(
    const std::string& key,
    size_t connectionSize) const {
  if (auto factory = m_producerConnections.find(key);
      factory != m_producerConnections.end())
    return factory->second(connectionSize);

  throw PipelineRegistryException(
      std::string("key ") + key +
      " was not presented in producer connections registry");
}

std::shared_ptr<StageConnection> PipelineRegistry::constructConsumerConnection(
    const std::string& key,
    size_t connectionSize) const {
  if (auto factory = m_consumerConnections.find(key);
      factory != m_consumerConnections.end())
    return factory->second(connectionSize);

  throw PipelineRegistryException(
      std::string("key ") + key +
      " was not presented in consumer connections registry");
}

std::shared_ptr<IPipelineStage> PipelineRegistry::constructProducer(
    const std::string& key,
    std::shared_ptr<StageConnection> connection) const {
  auto producersFactory = m_producers.find(key);
  if (producersFactory != m_producers.end()) {
    return producersFactory->second(connection);
  }

  throw PipelineRegistryException(std::string("key ") + key +
                                  " was not presented in producers registry");
}

std::shared_ptr<IPipelineStage> PipelineRegistry::constructConsumer(
    const std::string& key,
    ConsumptionStrategy strategy,
    std::shared_ptr<StageConnection> inConnection) const {
  auto consumerFactory = m_consumers.find(key);
  if (consumerFactory != m_consumers.end()) {
    return consumerFactory->second(strategy, inConnection);
  }

  throw PipelineRegistryException(std::string("key ") + key +
                                  " was not presented in producers registry");
}

std::shared_ptr<IPipelineStage> PipelineRegistry::constructConsumerAndProducer(
    const std::string& key,
    ConsumptionStrategy strategy,
    std::shared_ptr<StageConnection> inConnection,
    std::shared_ptr<StageConnection> outConnection) const {
  auto factory = m_consumersProducers.find(key);
  if (factory != m_consumersProducers.end()) {
    return factory->second(strategy, inConnection, outConnection);
  }

  throw PipelineRegistryException(std::string("key ") + key +
                                  " was not presented in producers registry");
}
