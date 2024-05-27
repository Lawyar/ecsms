#pragma once

#include "PipelineStage.h"
#include "ProducerStage.h"
#include "ConsumerStage.h"
#include "ProducerAndConsumerStage.h"
#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "InOutStageConnection.h"
#include "PipelineStageObserverException.h"

#include <functional>
#include <memory>
#include <unordered_map>

struct ConsumerAndProducerConnection {
  std::shared_ptr<StageConnection> in;
  std::shared_ptr<StageConnection> out;
};

class PipelineRegistry {
public:
  using ProducerStageFactory = std::function<std::shared_ptr<PipelineStage>(std::shared_ptr<StageConnection>)>;
  using ConsumerStageFactory = std::function<std::shared_ptr<PipelineStage>(TaskRetrieveStrategy, std::shared_ptr<StageConnection>)>;
  using ConsumerAndProducerStageFactory = std::function<std::shared_ptr<PipelineStage>(TaskRetrieveStrategy, std::shared_ptr<StageConnection>, std::shared_ptr<StageConnection>)>;
  using ProducerConnectionFactory = std::function<std::shared_ptr<StageConnection>(size_t)>;
  using ConsumerConnectionFactory = std::function<std::shared_ptr<StageConnection>(size_t)>;
  using ConsumerAndProducerConnectionFactory = std::function<ConsumerAndProducerConnection(size_t)>;

  //template <typename Derived> void registerClass(const Key &key);

  template<typename ProducerT>
  void registerProducer();

  template <typename ProducerT>
  void registerProducerFactory(const ProducerStageFactory factory);

  template <typename ProducerT> void registerProducerConnection();

  void registerFactory(const std::string &key, const ConsumerStageFactory factory);
  void registerFactory(const std::string &key,
                       const ConsumerAndProducerStageFactory factory);

  std::shared_ptr<PipelineStage> constructProducer(const std::string &key,
                                           std::shared_ptr<StageConnection> connection);

  // 
  std::shared_ptr<StageConnection>
  constructProducerConnection(const std::string &key, size_t connectionSize);

  std::shared_ptr<StageConnection>
  constructConsumerConnection(const std::string &key, size_t connectionSize);

  ConsumerAndProducerConnection
  constructConsumerAndProducerConnection(const std::string &key, size_t connectionSize);

  //std::shared_ptr<PipelineStage> constructProducerStage(const Key &key,
  //                                                 Args... args) const;
  //std::shared_ptr<PipelineStage> constructConsumerStage(const Key &key,
  //                                                 Args... args) const;
  //std::shared_ptr<PipelineStage> constructConsumerAndProducerStage(const Key &key,
  //                                                 Args... args) const;

private:
  //template <typename Derived>
  //static std::shared_ptr<Base> constructDerived(Args... args);

  std::unordered_map<std::string, ProducerStageFactory> m_producers;
  std::unordered_map<std::string, ConsumerStageFactory> m_consumers;
  std::unordered_map<std::string, ConsumerAndProducerStageFactory>
      m_consumersProducers;

  std::unordered_map<std::string, ProducerConnectionFactory> m_producerConnections;
  std::unordered_map<std::string, ConsumerConnectionFactory>
      m_consumerConnections;
  std::unordered_map<std::string, ConsumerAndProducerConnectionFactory>
      m_consumerAndProducerConnections;
};

/*template <typename Base, typename Key, typename... Args>
template <typename Derived>
void Registry<Base, Key, Args...>::registerClass(const Key &key) {
  m_map[key] = &constructDerived<Derived>;
}*/

//template <typename Base, typename Key, typename... Args>
//void Registry<Base, Key, Args...>::registerFactory(const Key &key,
//                                                   const FactoryFunc factory) {
//  m_map[key] = factory;
//}
//
//template <typename Base, typename Key, typename... Args>
//std::shared_ptr<Base>
//Registry<Base, Key, Args...>::construct(const Key &key, Args... args) const {
//  return m_map.at(key)(std::forward<Args>(args)...);
//}
//
//template <typename Base, typename Key, typename... Args>
//template <typename Derived>
//std::shared_ptr<Base>
//Registry<Base, Key, Args...>::constructDerived(Args... args) {
//  return std::make_unique<Derived>(std::forward<Args>(args)...);
//}
//
//template <typename Base, typename Key, typename... Args>
//void Registry<Base, Key, Args...>::registerFactory(
//    const Key &key, const ProducerStageFactory factory) {
//  if (m_producers.find(key) != m_producers.end())
//    throw PipelineStageObserverException("stage was")
//}

template <typename ProducerT>
void PipelineRegistry::registerProducer() {
  registerProducerFactory<ProducerT>([](shared_ptr<StageConnection>
                                                    connection) {
    auto outConnection = dynamic_cast<OutStageConnection<typename ProducerT::productionT> *>(
        connection.get());
    return make_shared<ProducerT>(
        shared_ptr<OutStageConnection<typename ProducerT::productionT>>(
            outConnection));
  });
}

template <typename ProducerT>
void PipelineRegistry::registerProducerFactory(
    const ProducerStageFactory factory) {
  const auto &key = ProducerT::stageName;
  if (m_producers.find(key) != m_producers.end() ||
      m_consumers.find(key) != m_consumers.end() ||
      m_consumersProducers.find(key) != m_consumersProducers.end())
    throw PipelineStageObserverException("stage has already been added");
  m_producers[key] = factory;
  registerProducerConnection<ProducerT>();
}

template <typename ProducerT>
void PipelineRegistry::registerProducerConnection() {
  const auto &key = ProducerT::stageName;
  if (m_producerConnections.find(key) != m_producerConnections.end() ||
      m_consumerConnections.find(key) != m_consumerConnections.end() ||
      m_consumerAndProducerConnections.find(key) !=
          m_consumerAndProducerConnections.end())
    throw PipelineStageObserverException("connection has already been added");

  m_producerConnections[key] = [](size_t connectionSize) {
    return std::make_shared<
        InOutStageConnection<typename ProducerT::productionT>>(connectionSize);
  };
}

std::shared_ptr<PipelineStage>
PipelineRegistry::constructProducer(const std::string &key,
                         std::shared_ptr<StageConnection> connection) {
  auto factory = m_producers.find(key);
  if (factory != m_producers.end()) {
    return factory->second(connection);
  }
}

std::shared_ptr<StageConnection>
PipelineRegistry::constructProducerConnection(const std::string &key,
                                      size_t connectionSize) {
  if (auto factory = m_producerConnections.find(key);
      factory != m_producerConnections.end())
    return factory->second(connectionSize);
}

std::shared_ptr<StageConnection>
PipelineRegistry::constructConsumerConnection(const std::string &key,
                                              size_t connectionSize) {
  if (auto factory = m_consumerConnections.find(key);
      factory != m_consumerConnections.end())
    return factory->second(connectionSize);
}

ConsumerAndProducerConnection
PipelineRegistry::constructConsumerAndProducerConnection(
    const std::string &key,
    size_t connectionSize) {
  if (auto factory = m_consumerAndProducerConnections.find(key);
      factory != m_consumerAndProducerConnections.end())
    return factory->second(connectionSize);
}
