#pragma once

#include "IPipelineStage.h"
#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineException.h"
#include "SteadyClock.h"
#include "TaskRetrieveStrategy.h"

#include <iostream>
#include <optional>

template <typename In, typename Out>
class GenericPipelineStage : public IPipelineStage {
 public:
  GenericPipelineStage(const std::string_view stageName,
                       std::optional<ConsumerStrategy>,
                       std::weak_ptr<InStageConnection<In>>,
                       std::weak_ptr<OutStageConnection<Out>>);

  ~GenericPipelineStage() override;

  void run() override;

  void shutdown() override;

 protected:
  void setConsumerId(size_t consumerId) override;

  std::shared_ptr<In> getConsumptionData();
  void releaseConsumptionData(std::shared_ptr<In> taskData);

  std::shared_ptr<Out> getProductionData();
  void releaseProductionData(std::shared_ptr<Out> taskData, bool produced);

  virtual void consumeAndProduce(std::shared_ptr<In> inData,
                                 std::shared_ptr<Out> outData) = 0;

 private:
  void releaseTasksOnError(std::shared_ptr<In> inData,
                           std::shared_ptr<Out> outData);

  bool inConnectionIsShutdown();

  bool outConnectionIsShutdown();

 private:
  std::atomic_bool m_shutdownSignaled;
  std::thread m_thread;

  const std::optional<ConsumerStrategy> m_consumerStrategy;
  std::optional<size_t> m_consumerId;
  size_t m_lastConusmedTaskId;

  std::weak_ptr<InStageConnection<In>> m_inConnection;
  std::weak_ptr<OutStageConnection<Out>> m_outConnection;
};

template <typename In, typename Out>
GenericPipelineStage<In, Out>::GenericPipelineStage(
    const std::string_view stageName,
    std::optional<ConsumerStrategy> consumerStrategy,
    std::weak_ptr<InStageConnection<In>> inConnection,
    std::weak_ptr<OutStageConnection<Out>> outConnection)
    : IPipelineStage(stageName),
      m_shutdownSignaled{false},
      m_consumerStrategy(consumerStrategy),
      m_consumerId(std::nullopt),
      m_inConnection(inConnection),
      m_outConnection(outConnection),
      m_lastConusmedTaskId{0} {
  if (!inConnection.expired() && !consumerStrategy.has_value())
    throw std::invalid_argument("consumerStrategy is null");

  if (inConnection.expired() && consumerStrategy.has_value())
    throw std::invalid_argument(
        "consumerStrategy is presented without inConnection");

  if (inConnection.expired() && outConnection.expired())
    throw std::invalid_argument("inConnection and outConnection are null");
}

template <typename In, typename Out>
GenericPipelineStage<In, Out>::~GenericPipelineStage() {
  shutdown();
}

template <typename In, typename Out>
void GenericPipelineStage<In, Out>::run() {
  if (!m_inConnection.expired() && !m_consumerId.has_value())
    throw PipelineException(std::string("consumerId not set for stage ") +
                            getName());

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
void GenericPipelineStage<In, Out>::shutdown() {
  m_shutdownSignaled = true;

  if (m_thread.joinable())
    m_thread.join();
}

template <typename In, typename Out>
void GenericPipelineStage<In, Out>::setConsumerId(size_t consumerId) {
  if (m_inConnection.expired())
    throw PipelineException("m_inConnection expired");

  m_consumerId = consumerId;
}

template <typename In, typename Out>
std::shared_ptr<In> GenericPipelineStage<In, Out>::getConsumptionData() {
  std::shared_ptr<In> inData;
  if (auto in = m_inConnection.lock(); in != nullptr) {
    auto inTask = in->getConsumerTask(
        m_consumerId.value(), m_consumerStrategy.value(), m_lastConusmedTaskId);
    if (!inTask)
      return nullptr;

    m_lastConusmedTaskId = inTask->taskId;
    inData = inTask->data;
  }

  return inData;
}

template <typename In, typename Out>
void GenericPipelineStage<In, Out>::releaseConsumptionData(
    std::shared_ptr<In> taskData) {
  if (!taskData)
    throw std::invalid_argument("taskData is null");

  if (auto in = m_inConnection.lock(); in != nullptr)
    in->releaseConsumerTask(taskData, m_consumerId.value());
  else
    throw PipelineException("m_inConnection expired");
}

template <typename In, typename Out>
std::shared_ptr<Out> GenericPipelineStage<In, Out>::getProductionData() {
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
void GenericPipelineStage<In, Out>::releaseProductionData(
    std::shared_ptr<Out> taskData,
    bool produced) {
  if (!taskData)
    throw std::invalid_argument("taskData is null");

  static size_t taskId = 1;

  if (auto out = m_outConnection.lock(); out != nullptr)
    out->releaseProducerTask(taskData, taskId++, produced);
  else
    throw PipelineException("m_outConnection expired");
}

template <typename In, typename Out>
void GenericPipelineStage<In, Out>::releaseTasksOnError(
    std::shared_ptr<In> inData,
    std::shared_ptr<Out> outData) {
  if (inData && !m_inConnection.expired())
    releaseConsumptionData(inData);

  if (outData && !m_outConnection.expired())
    releaseProductionData(outData, false);
}

template <typename In, typename Out>
bool GenericPipelineStage<In, Out>::inConnectionIsShutdown() {
  auto in = m_inConnection.lock();
  if (in != nullptr && in->isShutdown())
    return true;

  return false;
}

template <typename In, typename Out>
bool GenericPipelineStage<In, Out>::outConnectionIsShutdown() {
  auto out = m_outConnection.lock();
  if (out != nullptr && out->isShutdown())
    return true;

  return false;
}
