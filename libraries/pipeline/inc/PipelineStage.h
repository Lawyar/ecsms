#pragma once

#include "ConsumptionStrategy.h"
#include "IPipelineStage.h"
#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineException.h"
#include "SteadyClock.h"

#include <iostream>
#include <optional>

template <typename In, typename Out>
class PipelineStage : public IPipelineStage {
 public:
  PipelineStage(const std::string_view stageName,
                std::optional<ConsumptionStrategy>,
                std::weak_ptr<InStageConnection<In>>,
                std::weak_ptr<OutStageConnection<Out>>);

  ~PipelineStage() override;

  void run() override;

  void shutdown() override;

  PipelineStageType getStageType() const override;

  std::optional<ConsumptionStrategy> getConsumptionStrategy() const override;

 protected:
  virtual void consumeAndProduce(std::shared_ptr<In> inData,
                                 std::shared_ptr<Out> outData) = 0;

  std::shared_ptr<In> getConsumptionData();
  void dataConsumed(std::shared_ptr<In> taskData, bool consumed = true);

  std::shared_ptr<Out> getProductionData();
  void dataProduced(std::shared_ptr<Out> taskData, bool produced = true);

  void setConsumerId(size_t consumerId);

 private:
  void releaseTasksOnError(std::shared_ptr<In> inData,
                           std::shared_ptr<Out> outData);

  bool inConnectionIsShutdown() const;

  bool outConnectionIsShutdown() const;

 private:
  std::atomic_bool m_shutdownSignaled;
  std::thread m_thread;

  std::optional<ConsumptionStrategy> m_consumptionStrategy;
  std::optional<size_t> m_consumerId;
  size_t m_lastConusmedTaskId;

  std::weak_ptr<InStageConnection<In>> m_inConnection;
  std::weak_ptr<OutStageConnection<Out>> m_outConnection;
};

template <typename In, typename Out>
PipelineStage<In, Out>::PipelineStage(
    const std::string_view stageName,
    std::optional<ConsumptionStrategy> consumptionStrategy,
    std::weak_ptr<InStageConnection<In>> inConnection,
    std::weak_ptr<OutStageConnection<Out>> outConnection)
    : IPipelineStage(stageName),
      m_shutdownSignaled{false},
      m_consumptionStrategy(consumptionStrategy),
      m_consumerId(std::nullopt),
      m_inConnection(inConnection),
      m_outConnection(outConnection),
      m_lastConusmedTaskId{0} {
  if (!inConnection.expired() && !consumptionStrategy.has_value())
    throw std::invalid_argument("consumerStrategy is null");

  if (inConnection.expired() && consumptionStrategy.has_value())
    throw std::invalid_argument(
        "inConnection expired");

  if (inConnection.expired() && outConnection.expired())
    throw std::invalid_argument("inConnection and outConnection are null");
}

template <typename In, typename Out>
PipelineStage<In, Out>::~PipelineStage() {
  shutdown();
}

template <typename In, typename Out>
void PipelineStage<In, Out>::run() {
  if (!m_inConnection.expired() && !m_consumerId.has_value())
    throw PipelineException(std::string("consumerId is null"));

  m_thread = std::thread([this] {
    while (!m_shutdownSignaled) {
      std::shared_ptr<In> inData;
      std::shared_ptr<Out> outData;
      try {
        inData = getConsumptionData();
        outData = getProductionData();

        if (!inData && inConnectionIsShutdown())
          break;
        else if (!outData && outConnectionIsShutdown())
          break;
        consumeAndProduce(inData, outData);

        inData = nullptr;
        outData = nullptr;
      } catch (std::exception& ex) {
        std::cerr << "ConnectablePipelineStage: " << ex.what() << std::endl;
      } catch (...) {
        std::cerr << "ConnectablePipelineStage: "
                  << "unhandled exception in stage function" << std::endl;
      }

      releaseTasksOnError(inData, outData);
    }
  });
}

template <typename In, typename Out>
void PipelineStage<In, Out>::shutdown() {
  m_shutdownSignaled = true;

  if (m_thread.joinable())
    m_thread.join();
}

template <typename In, typename Out>
PipelineStageType PipelineStage<In, Out>::getStageType() const {
  if (!m_inConnection.expired() && !m_outConnection.expired())
    return PipelineStageType::producerConsumer;
  else if (m_inConnection.expired())
    return PipelineStageType::producer;
  else if (m_outConnection.expired())
    return PipelineStageType::consumer;

  throw PipelineException("cannot determine PipelineStageType");
}

template <typename In, typename Out>
std::optional<ConsumptionStrategy>
PipelineStage<In, Out>::getConsumptionStrategy() const {
  return m_consumptionStrategy;
}

template <typename In, typename Out>
void PipelineStage<In, Out>::setConsumerId(size_t consumerId) {
  if (m_inConnection.expired())
    throw PipelineException("m_inConnection expired");

  m_consumerId = consumerId;
}

template <typename In, typename Out>
std::shared_ptr<In> PipelineStage<In, Out>::getConsumptionData() {
  std::shared_ptr<In> inData;
  if (auto in = m_inConnection.lock(); in != nullptr) {
    auto inTask =
        in->getConsumerTask(m_consumerId.value(), m_consumptionStrategy.value(),
                            m_lastConusmedTaskId);
    if (!inTask)
      return nullptr;

    m_lastConusmedTaskId = inTask->taskId;
    inData = inTask->data;
  }

  return inData;
}

template <typename In, typename Out>
void PipelineStage<In, Out>::dataConsumed(
    std::shared_ptr<In> taskData, bool consumed) {
  if (!taskData)
    throw std::invalid_argument("taskData is null");

  if (auto in = m_inConnection.lock(); in != nullptr)
    in->taskConsumed(taskData, m_consumerId.value(), consumed);
  else
    throw PipelineException("m_inConnection expired");
}

template <typename In, typename Out>
std::shared_ptr<Out> PipelineStage<In, Out>::getProductionData() {
  std::shared_ptr<Out> outData;

  if (auto out = m_outConnection.lock(); out != nullptr) {
    auto outTask = out->getProducerTask();
    if (!outTask)
      return nullptr;

    outData = outTask->data;
  }

  return outData;
}

template <typename In, typename Out>
void PipelineStage<In, Out>::dataProduced(
    std::shared_ptr<Out> taskData, bool produced) {
  if (!taskData)
    throw std::invalid_argument("taskData is null");

  static size_t taskId = 1;

  if (auto out = m_outConnection.lock(); out != nullptr)
    out->taskProduced(taskData, taskId++, produced);
  else
    throw PipelineException("m_outConnection expired");
}

template <typename In, typename Out>
void PipelineStage<In, Out>::releaseTasksOnError(std::shared_ptr<In> inData,
                                                 std::shared_ptr<Out> outData) {
  if (inData && !m_inConnection.expired())
    dataConsumed(inData, false);

  if (outData && !m_outConnection.expired())
    dataProduced(outData, false);
}

template <typename In, typename Out>
bool PipelineStage<In, Out>::inConnectionIsShutdown() const {
  auto in = m_inConnection.lock();
  if (in != nullptr && in->isShutdown())
    return true;

  return false;
}

template <typename In, typename Out>
bool PipelineStage<In, Out>::outConnectionIsShutdown() const {
  auto out = m_outConnection.lock();
  if (out != nullptr && out->isShutdown())
    return true;

  return false;
}
