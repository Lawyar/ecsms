#pragma once

#include "ConsumerStage.h"
#include "IPipelineStage.h"
#include "InOutStageConnection.h"
#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineRegistryException.h"
#include "ProducerAndConsumerStage.h"
#include "ProducerStage.h"
#include "PipelineStageType.h"

#include <functional>
#include <memory>
#include <unordered_map>

struct ConsumerAndProducerConnection {
  std::shared_ptr<StageConnection> in;
  std::shared_ptr<StageConnection> out;
};

class PipelineRegistry {
 public:
  using ProducerStageFactory = std::function<std::shared_ptr<IPipelineStage>(
      std::shared_ptr<StageConnection>)>;
  using ConsumerStageFactory = std::function<std::shared_ptr<IPipelineStage>(
      ConsumerStrategy,
      std::shared_ptr<StageConnection>)>;
  using ConsumerAndProducerStageFactory =
      std::function<std::shared_ptr<IPipelineStage>(
          ConsumerStrategy,
          std::shared_ptr<StageConnection>,
          std::shared_ptr<StageConnection>)>;
  using ProducerConnectionFactory =
      std::function<std::shared_ptr<StageConnection>(size_t)>;
  using ConsumerConnectionFactory =
      std::function<std::shared_ptr<StageConnection>(size_t)>;
  using ConsumerAndProducerConnectionFactory =
      std::function<ConsumerAndProducerConnection(size_t)>;

  template <typename ProducerT>
  void registerProducer(const std::string& key);

  template <typename ProducerT>
  void registerProducerFactory(const std::string& key,
                               const ProducerStageFactory factory);

  template <typename ConsumerT>
  void registerConsumer(const std::string& key);

  template <typename ConsumerT>
  void registerConsumerFactory(const std::string& key,
                               const ConsumerStageFactory factory);

  template <typename ConsumerAndProducerT>
  void registerConsumerAndProducer(const std::string& key);

  template <typename ConsumerAndProducerT>
  void registerConsumerAndProducerFactory(const std::string& key,
                                          const ConsumerAndProducerStageFactory factory);

  PipelineStageType getStageType(const std::string& key);

  std::shared_ptr<StageConnection> constructProducerConnection(
      const std::string& key,
      size_t connectionSize);

  std::shared_ptr<StageConnection> constructConsumerConnection(
      const std::string& key,
      size_t connectionSize);

  std::shared_ptr<IPipelineStage> constructProducer(
      const std::string& key,
      std::shared_ptr<StageConnection> outConnection);


  std::shared_ptr<IPipelineStage> constructConsumer(
      const std::string& key,
      ConsumerStrategy strategy,
      std::shared_ptr<StageConnection> inConnection);

  std::shared_ptr<IPipelineStage> constructConsumerAndProducer(
      const std::string& key,
      ConsumerStrategy strategy,
      std::shared_ptr<StageConnection> inConnection,
      std::shared_ptr<StageConnection> outConnection);

 private:
  PipelineRegistry();

  template <typename ProducerT>
  void registerProducerConnection();

  template <typename ConsumerT>
  void registerConsumerConnection();

 private:
  std::unordered_map<std::string, ProducerStageFactory> m_producers;
  std::unordered_map<std::string, ConsumerStageFactory> m_consumers;
  std::unordered_map<std::string, ConsumerAndProducerStageFactory>
      m_consumersProducers;

  std::unordered_map<std::string, ProducerConnectionFactory>
      m_producerConnections;
  std::unordered_map<std::string, ConsumerConnectionFactory>
      m_consumerConnections;
  std::unordered_map<std::string, ConsumerAndProducerConnectionFactory>
      m_consumerAndProducerConnections;
};

template <typename ProducerT>
void PipelineRegistry::registerProducer(const std::string& key) {
  registerProducerFactory<ProducerT>(
      key, [](shared_ptr<StageConnection> connection) {
        auto outConnection = std::dynamic_pointer_cast<
            OutStageConnection<typename ProducerT::productionT>>(connection);
        return make_shared<ProducerT>(outConnection);
      });
}

template <typename ProducerT>
void PipelineRegistry::registerProducerFactory(
    const std::string& key,
    const ProducerStageFactory factory) {
  if (m_producers.find(key) != m_producers.end() ||
      m_consumers.find(key) != m_consumers.end() ||
      m_consumersProducers.find(key) != m_consumersProducers.end())
    throw PipelineRegistryException("stage has already been added");
  m_producers[key] = factory;
  registerProducerConnection<ProducerT>();
}

template <typename ProducerT>
void PipelineRegistry::registerProducerConnection() {
  const auto& key = ProducerT::stageName;
  if (m_producerConnections.find(key) != m_producerConnections.end() ||
      m_consumerConnections.find(key) != m_consumerConnections.end() ||
      m_consumerAndProducerConnections.find(key) !=
          m_consumerAndProducerConnections.end())
    throw PipelineRegistryException("connection has already been added");

  m_producerConnections[key] = [](size_t connectionSize) {
    return std::make_shared<
        InOutStageConnection<typename ProducerT::productionT>>(connectionSize);
  };
}

std::shared_ptr<IPipelineStage> PipelineRegistry::constructProducer(
    const std::string& key,
    std::shared_ptr<StageConnection> connection) {
  auto producersFactory = m_producers.find(key);
  if (producersFactory != m_producers.end()) {
    return producersFactory->second(connection);
  }

  auto consumersFactory = m_consumers.find(key);
  if (consumersFactory != m_consumers.end()) {
    return consumersFactory->second(connection);
  }

  auto consumersFactory = m_consumers.find(key);
  if (consumersFactory != m_consumers.end()) {
    return consumersFactory->second(connection);
  }
}

PipelineStageType PipelineRegistry::getStageType(const std::string& key) {
  if (m_producers.find(key) != m_producers.end())
    return PipelineStageType::producer;
  else if (m_consumers.find(key) != m_consumers.end())
    return PipelineStageType::consumer;
  else if (m_consumersProducers.find(key) != m_consumersProducers.end()) {
    return PipelineStageType::producerConsumer;
  }
  
  throw PipelineRegistryException(std::string("key ") + key + " was not presented in registry");
}

std::shared_ptr<StageConnection> PipelineRegistry::constructProducerConnection(
    const std::string& key,
    size_t connectionSize) {
  if (auto factory = m_producerConnections.find(key);
      factory != m_producerConnections.end())
    return factory->second(connectionSize);

  throw PipelineRegistryException(std::string("key ") + key + " was not presented in producer connections registry");
}

std::shared_ptr<StageConnection> PipelineRegistry::constructConsumerConnection(
    const std::string& key,
    size_t connectionSize) {
  if (auto factory = m_consumerConnections.find(key);
      factory != m_consumerConnections.end())
    return factory->second(connectionSize);

  throw PipelineRegistryException(std::string("key ") + key + " was not presented in consumer connections registry");
}

ConsumerAndProducerConnection
PipelineRegistry::constructConsumerAndProducerConnection(
    const std::string& key,
    size_t connectionSize) {
  if (auto factory = m_consumerAndProducerConnections.find(key);
      factory != m_consumerAndProducerConnections.end())
    return factory->second(connectionSize);

  throw PipelineRegistryException(std::string("key ") + key + " was not presented in consumer and producer connections registry");
}
