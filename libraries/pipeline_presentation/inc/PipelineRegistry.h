#pragma once

#include "ConsumerStage.h"
#include "IPipelineStage.h"
#include "InOutStageConnection.h"
#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineRegistryException.h"
#include "PipelineStageType.h"
#include "ProducerAndConsumerStage.h"
#include "ProducerStage.h"

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
      ConsumptionStrategy,
      std::shared_ptr<StageConnection>)>;
  using ConsumerAndProducerStageFactory =
      std::function<std::shared_ptr<IPipelineStage>(
          ConsumptionStrategy,
          std::shared_ptr<StageConnection>,
          std::shared_ptr<StageConnection>)>;
  using ProducerConnectionFactory =
      std::function<std::shared_ptr<StageConnection>(size_t)>;
  using ConsumerConnectionFactory =
      std::function<std::shared_ptr<StageConnection>(size_t)>;
  using ConsumerAndProducerConnectionFactory =
      std::function<ConsumerAndProducerConnection(size_t)>;

  static PipelineRegistry& Instance();

  void reset();

  std::vector<std::string> getStageNames() const;

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
  void registerConsumerAndProducerFactory(
      const std::string& key,
      const ConsumerAndProducerStageFactory factory);

  PipelineStageType getStageType(const std::string& key) const;

  std::shared_ptr<StageConnection> constructProducerConnection(
      const std::string& key,
      size_t connectionSize) const;

  std::shared_ptr<StageConnection> constructConsumerConnection(
      const std::string& key,
      size_t connectionSize) const;

  std::shared_ptr<IPipelineStage> constructProducer(
      const std::string& key,
      std::shared_ptr<StageConnection> outConnection) const;

  std::shared_ptr<IPipelineStage> constructConsumer(
      const std::string& key,
      ConsumptionStrategy strategy,
      std::shared_ptr<StageConnection> inConnection) const;

  std::shared_ptr<IPipelineStage> constructConsumerAndProducer(
      const std::string& key,
      ConsumptionStrategy strategy,
      std::shared_ptr<StageConnection> inConnection,
      std::shared_ptr<StageConnection> outConnection) const;

 private:
  PipelineRegistry();

  static void Init();

  template <typename ProducerT>
  void registerProducerConnection(const std::string& key);

  template <typename ConsumerT>
  void registerConsumerConnection(const std::string& key);

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

private:
  static std::atomic_bool initialized;
  static PipelineRegistry globalRegistry;
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
  registerProducerConnection<ProducerT>(key);
}

template <typename ConsumerT>
void PipelineRegistry::registerConsumer(const std::string& key) {
  registerConsumerFactory<ConsumerT>(
      key,
      [](ConsumptionStrategy strategy, shared_ptr<StageConnection> connection) {
        auto inConnection = std::dynamic_pointer_cast<
            InStageConnection<typename ConsumerT::consumptionT>>(connection);
        return make_shared<ConsumerT>(strategy, inConnection);
      });
}

template <typename ConsumerT>
void PipelineRegistry::registerConsumerFactory(
    const std::string& key,
    const ConsumerStageFactory factory) {
  if (m_producers.find(key) != m_producers.end() ||
      m_consumers.find(key) != m_consumers.end() ||
      m_consumersProducers.find(key) != m_consumersProducers.end())
    throw PipelineRegistryException("stage has already been added");
  m_consumers[key] = factory;
  registerConsumerConnection<ConsumerT>(key);
}

template <typename ConsumerAndProducerT>
void PipelineRegistry::registerConsumerAndProducer(const std::string& key) {
  registerConsumerAndProducerFactory<ConsumerAndProducerT>(
      key, [](ConsumptionStrategy strategy,
              std::shared_ptr<StageConnection> inConnection,
              std::shared_ptr<StageConnection> outConnection) {
        auto in = std::dynamic_pointer_cast<
            InStageConnection<typename ConsumerAndProducerT::consumptionT>>(
            inConnection);
        auto out = std::dynamic_pointer_cast<
            OutStageConnection<typename ConsumerAndProducerT::productionT>>(
            outConnection);
        return std::make_shared<ConsumerAndProducerT>(strategy, in, out);
      });
}

template <typename ConsumerAndProducerT>
void PipelineRegistry::registerConsumerAndProducerFactory(
    const std::string& key,
    const ConsumerAndProducerStageFactory factory) {
  if (m_producers.find(key) != m_producers.end() ||
      m_consumers.find(key) != m_consumers.end() ||
      m_consumersProducers.find(key) != m_consumersProducers.end())
    throw PipelineRegistryException("stage has already been added");
  m_consumersProducers[key] = factory;
  registerConsumerConnection<ConsumerAndProducerT>(key);
  registerProducerConnection<ConsumerAndProducerT>(key);
}

template <typename ProducerT>
void PipelineRegistry::registerProducerConnection(const std::string& key) {
  if (m_producerConnections.find(key) != m_producerConnections.end())
    throw PipelineRegistryException("connection has already been added");

  m_producerConnections[key] = [](size_t connectionSize) {
    return std::make_shared<
        InOutStageConnection<typename ProducerT::productionT>>(connectionSize);
  };
}

template <typename ConsumerT>
void PipelineRegistry::registerConsumerConnection(const std::string& key) {
  if (m_consumerConnections.find(key) != m_consumerConnections.end())
    throw PipelineRegistryException("connection has already been added");

  m_consumerConnections[key] = [](size_t connectionSize) {
    return std::make_shared<
        InOutStageConnection<typename ConsumerT::consumptionT>>(connectionSize);
  };
}
