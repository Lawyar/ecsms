#pragma once

#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineException.h"
#include "PipelineStage.h"
#include "SteadyClock.h"
#include "TaskRetrieveStrategy.h"

#include <iostream>
#include <optional>

template <typename In, typename Out>
class ConnectablePipelineStage : public PipelineStage {
public:
  ConnectablePipelineStage(const std::string_view stageName,
                           std::optional<TaskRetrieveStrategy>,
                           std::weak_ptr<InStageConnection<In>>,
                           std::weak_ptr<OutStageConnection<Out>>,
                           std::optional<std::any> = std::nullopt);

  void run() override;

  PipelineStageType getStageType() override;
  bool consumerTasksAvailable() override;

protected:
  void setConsumerId(size_t consumerId);
  void releaseConsumerTask(std::shared_ptr<In> taskData);
  void releaseProducerTask(std::shared_ptr<Out> taskData, bool produced = true);

private:
  virtual void consumeAndProduce(std::shared_ptr<In> inData,
                                 std::shared_ptr<Out> outData) = 0;

protected:
  std::optional<std::any> m_stageParameters;

private:
  const std::optional<TaskRetrieveStrategy> m_consumerStrategy;

  std::optional<size_t> m_consumerId;
  std::weak_ptr<InStageConnection<In>> m_inConnection;
  std::weak_ptr<OutStageConnection<Out>> m_outConnection;

  uint64_t m_leastTimestamp;
};

template <typename In, typename Out>
ConnectablePipelineStage<In, Out>::ConnectablePipelineStage(
    const std::string_view stageName,
    std::optional<TaskRetrieveStrategy> consumerStrategy,
    std::weak_ptr<InStageConnection<In>> inConnection,
    std::weak_ptr<OutStageConnection<Out>> outConnection,
    std::optional<std::any> stageParameters)
    : PipelineStage(stageName), m_stageParameters(stageParameters),
      m_consumerStrategy(consumerStrategy),
      m_consumerId(std::nullopt), m_inConnection(inConnection),
      m_outConnection(outConnection), m_leastTimestamp{0} {
  if (!inConnection.expired() && !consumerStrategy.has_value())
    throw std::invalid_argument("consumerStrategy is null");

  if (inConnection.expired() && consumerStrategy.has_value())
    throw std::invalid_argument(
        "consumerStrategy is presented without inConnection");

  if (inConnection.expired() && outConnection.expired())
    throw std::invalid_argument("inConnection and outConnection are null");
}

template <typename In, typename Out>
void ConnectablePipelineStage<In, Out>::run() {
  if (!m_inConnection.expired() && !m_consumerId.has_value())
    throw PipelineException(std::string("consumerId not set for stage ") +
                            getName());

  m_thread = std::thread([this] {
    while (!m_shutdownSignaled) {
      std::shared_ptr<In> inData;
      std::shared_ptr<Out> outData;

      if (auto in = m_inConnection.lock(); in != nullptr) {
        auto inTask = in->getConsumerTask(
            m_consumerId.value(), m_consumerStrategy.value(), m_leastTimestamp);
        if (!inTask)
          break;

        m_leastTimestamp = inTask->timestamp;
        inData = inTask->data;
      }

      if (auto out = m_outConnection.lock(); out != nullptr) {
        auto outTask = out->getProducerTask();
        if (!outTask)
          break;

        outData = outTask->data;
      }

      try {
        consumeAndProduce(inData, outData);

        inData = nullptr;
        outData = nullptr;
      } catch (std::exception &ex) {
        std::cerr << "ConnectablePipelineStage: " << ex.what() << std::endl;
      } catch (...) {
        std::cerr << "ConnectablePipelineStage: "
                  << "unhandled exception in stage function" << std::endl;
      }

      if (inData && !m_inConnection.expired())
        releaseConsumerTask(inData);

      if (outData && !m_outConnection.expired())
        releaseProducerTask(outData, false);
    }
  });
}

template <typename In, typename Out>
PipelineStageType
ConnectablePipelineStage<In, Out>::getStageType() {
  if (m_inConnection.expired())
    return PipelineStageType::producer;
  else if (m_outConnection.expired())
    return PipelineStageType::consumer;
  else
    return PipelineStageType::producerConsumer;
}

template <typename In, typename Out>
bool ConnectablePipelineStage<In, Out>::consumerTasksAvailable() {
  if (auto in = m_inConnection.lock(); in != nullptr)
    in->consumerTasksAvailable(m_consumerId.value(), m_leastTimestamp);
  else
    throw PipelineException("m_inConnection expired");
}

template <typename In, typename Out>
void ConnectablePipelineStage<In, Out>::setConsumerId(
    size_t consumerId) {
  if (m_inConnection.expired())
    throw PipelineException("m_inConnection expired");

  m_consumerId = consumerId;
}

template <typename In, typename Out>
void ConnectablePipelineStage<In, Out>::releaseConsumerTask(
    std::shared_ptr<In> taskData) {
  if (!taskData)
    throw std::invalid_argument("taskData is null");

  if (auto in = m_inConnection.lock(); in != nullptr)
    in->releaseConsumerTask(taskData, m_consumerId.value());
  else
    throw PipelineException("m_inConnection expired");
}

template <typename In, typename Out>
void ConnectablePipelineStage<In, Out>::releaseProducerTask(
    std::shared_ptr<Out> taskData, bool produced) {
  if (!taskData)
    throw std::invalid_argument("taskData is null");

  auto time = SteadyClock::nowUs().count();

  if (auto out = m_outConnection.lock(); out != nullptr)
    out->releaseProducerTask(taskData, time, produced);
  else
    throw PipelineException("m_outConnection expired");
}
