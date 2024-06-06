#include "PipelineRegistry.h"

using namespace std;

PipelineRegistry::PipelineRegistry() {}

PipelineRegistry& PipelineRegistry::Instance() {
  static PipelineRegistry registry;
  return registry;
}

void PipelineRegistry::reset() {
  m_producers.clear();
  m_consumers.clear();
  m_consumersProducers.clear();
  m_producerConnections.clear();
  m_consumerConnections.clear();
  m_consumerAndProducerConnections.clear();
}

PipelineStageType PipelineRegistry::getStageType(const std::string& key) {
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
    size_t connectionSize) {
  if (auto factory = m_producerConnections.find(key);
      factory != m_producerConnections.end())
    return factory->second(connectionSize);

  throw PipelineRegistryException(
      std::string("key ") + key +
      " was not presented in producer connections registry");
}

std::shared_ptr<StageConnection> PipelineRegistry::constructConsumerConnection(
    const std::string& key,
    size_t connectionSize) {
  if (auto factory = m_consumerConnections.find(key);
      factory != m_consumerConnections.end())
    return factory->second(connectionSize);

  throw PipelineRegistryException(
      std::string("key ") + key +
      " was not presented in consumer connections registry");
}

std::shared_ptr<IPipelineStage> PipelineRegistry::constructProducer(
    const std::string& key,
    std::shared_ptr<StageConnection> connection) {
  auto producersFactory = m_producers.find(key);
  if (producersFactory != m_producers.end()) {
    return producersFactory->second(connection);
  }

  throw PipelineRegistryException(std::string("key ") + key +
                                  " was not presented in producers registry");
}

std::shared_ptr<IPipelineStage> PipelineRegistry::constructConsumer(
    const std::string& key,
    ConsumerStrategy strategy,
    std::shared_ptr<StageConnection> inConnection) {
  auto consumerFactory = m_consumers.find(key);
  if (consumerFactory != m_consumers.end()) {
    return consumerFactory->second(strategy, inConnection);
  }

  throw PipelineRegistryException(std::string("key ") + key +
                                  " was not presented in producers registry");
}

std::shared_ptr<IPipelineStage> PipelineRegistry::constructConsumerAndProducer(
    const std::string& key,
    ConsumerStrategy strategy,
    std::shared_ptr<StageConnection> inConnection,
    std::shared_ptr<StageConnection> outConnection) {
  auto factory = m_consumersProducers.find(key);
  if (factory != m_consumersProducers.end()) {
    return factory->second(strategy, inConnection, outConnection);
  }

  throw PipelineRegistryException(std::string("key ") + key +
                                  " was not presented in producers registry");
}
