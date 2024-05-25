#pragma once

#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineException.h"
#include "PipelineStage.h"
#include "SteadyClock.h"
#include "TaskRetrieveStrategy.h"
#include "PipelineStageObserver.h"

#include <iostream>
#include <optional>

template <typename In, typename Out>
class ConnectablePipelineStage : public PipelineStage {
public:
  ConnectablePipelineStage(const std::string_view,
                           std::optional<TaskRetrieveStrategy>,
                           std::shared_ptr<InStageConnection<In>>,
                           std::shared_ptr<OutStageConnection<Out>>);

  void run() override;

  PipelineStageType getStageType() override;
  bool consumerTasksAvailable() override;
  std::shared_ptr<StageConnection> getInConnection() override;

protected:
  void setConsumerId(size_t consumerId);
  void releaseConsumerTask(std::shared_ptr<In> taskData);
  void releaseProducerTask(std::shared_ptr<Out> taskData, bool produced = true);

private:
  virtual void function(std::shared_ptr<In> inData,
                        std::shared_ptr<Out> outData) = 0;

private:
  const std::optional<TaskRetrieveStrategy> m_consumerStrategy;

  std::optional<size_t> m_consumerId;
  std::shared_ptr<InStageConnection<In>> m_inConnection;
  std::shared_ptr<OutStageConnection<Out>> m_outConnection;

  uint64_t m_leastTimestamp;
};

template <typename In, typename Out>
ConnectablePipelineStage<In, Out>::ConnectablePipelineStage(
    const std::string_view stageName,
    std::optional<TaskRetrieveStrategy> consumerStrategy,
    std::shared_ptr<InStageConnection<In>> inConnection,
    std::shared_ptr<OutStageConnection<Out>> outConnection)
    : PipelineStage(stageName), m_consumerStrategy(consumerStrategy),
      m_consumerId(std::nullopt), m_inConnection(inConnection),
      m_outConnection(outConnection), m_leastTimestamp{0} {
  if (inConnection != nullptr && !consumerStrategy.has_value())
    throw std::invalid_argument("consumer task retrieve strategy is null");

  if (inConnection == nullptr && consumerStrategy.has_value())
    throw std::invalid_argument(
        "consumer task retrieve strategy is presented without in connection");

  if (inConnection == nullptr && outConnection == nullptr)
    throw std::invalid_argument("both connections are null");
}

template <typename In, typename Out>
void ConnectablePipelineStage<In, Out>::run() {
  if (m_inConnection != nullptr && !m_consumerId.has_value())
    throw PipelineException(std::string("consumerId not set for stage ") +
                            getName());

  m_thread = std::thread([this] {
    while (!m_shutdownSignaled) {
      std::shared_ptr<In> inData;
      std::shared_ptr<Out> outData;

      if (m_inConnection) {
        auto inTask = m_inConnection->getConsumerTask(
            m_consumerId.value(), m_consumerStrategy.value(), m_leastTimestamp);
        if (!inTask)
          break;

        m_leastTimestamp = inTask->timestamp;
        inData = inTask->data;
      }

      if (m_outConnection) {
        auto outTask = m_outConnection->getProducerTask();
        if (!outTask)
          break;

        outData = outTask->data;
      }

      try {
        function(inData, outData);

        inData = nullptr;
        outData = nullptr;
      } catch (std::exception &ex) {
        std::cerr << "ConnectablePipelineStage: " << ex.what() << std::endl;
      } catch (...) {
        std::cerr << "ConnectablePipelineStage: "
                  << "unhandled exception in stage function" << std::endl;
      }

      if (inData && m_inConnection)
        releaseConsumerTask(inData);

      if (outData && m_outConnection)
        releaseProducerTask(outData, false);
    }
  });
}

template <typename In, typename Out>
PipelineStageType ConnectablePipelineStage<In, Out>::getStageType() {
  if (!m_inConnection)
    return PipelineStageType::producer;
  else if (!m_outConnection)
    return PipelineStageType::consumer;
  else
    return PipelineStageType::producerConsumer;
}

template <typename In, typename Out>
bool ConnectablePipelineStage<In, Out>::consumerTasksAvailable() {
  if (!m_inConnection)
    throw PipelineException("m_inConnection is null");

  return m_inConnection->consumerTasksAvailable(m_consumerId.value(),
                                                m_leastTimestamp);
}

template <typename In, typename Out>
std::shared_ptr<StageConnection>
ConnectablePipelineStage<In, Out>::getInConnection() {
  if (!m_inConnection)
    throw PipelineException("m_inConnection is null");

  return m_inConnection;
}

template <typename In, typename Out>
void ConnectablePipelineStage<In, Out>::setConsumerId(size_t consumerId) {
  if (m_inConnection == nullptr)
    throw PipelineException("m_inConnection is null");

  m_consumerId = consumerId;
}

template <typename In, typename Out>
void ConnectablePipelineStage<In, Out>::releaseConsumerTask(
    std::shared_ptr<In> taskData) {
  if (!m_inConnection)
    throw std::invalid_argument("m_inConnection is null");

  if (!taskData)
    throw std::invalid_argument("taskData is null");

  m_inConnection->releaseConsumerTask(taskData, m_consumerId.value());
}

template <typename In, typename Out>
void ConnectablePipelineStage<In, Out>::releaseProducerTask(
    std::shared_ptr<Out> taskData, bool produced) {
  if (!m_outConnection)
    throw std::invalid_argument("m_outConnection is null");

  if (!taskData)
    throw std::invalid_argument("taskData is null");

  auto time = SteadyClock::nowUs().count();
  m_outConnection->releaseProducerTask(taskData, time, produced);
}
