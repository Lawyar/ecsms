#pragma once

#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineException.h"
#include "StageTaskState.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>

// single producer multiple consumers connection
template <typename T>
class SPMCStageConnection : public InStageConnection<T>,
                             public OutStageConnection<T> {
 public:
  SPMCStageConnection(std::vector<std::shared_ptr<T>> data);

  SPMCStageConnection(size_t connectionSize);

  ~SPMCStageConnection() override;

  void shutdown() override;

  bool isShutdown() const override;

  std::shared_ptr<StageTask<T>> getProducerTask() override;

  void taskProduced(std::shared_ptr<T> taskData,
                    size_t taskId,
                    bool produced) override;

  std::shared_ptr<StageTask<T>> getConsumerTask(size_t consumerId,
                                                ConsumptionStrategy strategy,
                                                size_t minTaskId) override;

  void taskConsumed(std::shared_ptr<T> taskData,
                    size_t consumerId,
                    bool consumed) override;

  size_t connectConsumer() override;

 private:
  void setTaskState(size_t taskId, StageTaskState);

  std::optional<size_t> findTaskIndexToProduce(
      std::unique_lock<std::mutex>& lock);

  std::optional<size_t> findTaskIndexToConsume(
      std::unique_lock<std::mutex>& lock,
      size_t consumerId,
      ConsumptionStrategy strategy,
      size_t minTaskId);

  bool taskLocked(size_t taskId);

  static std::vector<std::shared_ptr<T>> initData(size_t size);

 private:
  static constexpr size_t maxConsumersCount = 32u;

  std::vector<std::shared_ptr<StageTask<T>>> m_tasks;

  std::shared_ptr<StageTask<T>> m_consumingTasks[maxConsumersCount];
  size_t m_consumingTasksId[maxConsumersCount];
  size_t m_consumersCount;
  std::vector<std::vector<StageTaskState>> m_consumersStates;

  std::shared_ptr<StageTask<T>> m_producingTask;
  size_t m_producingId;
  std::vector<bool> m_producersStates;

  std::mutex m_mutex;
  std::condition_variable m_waitConsumerTaskCv;
  std::condition_variable m_waitProducerTaskCv;
  std::atomic_bool m_shutdownSignaled;
};

template <typename T>
SPMCStageConnection<T>::SPMCStageConnection(
    std::vector<std::shared_ptr<T>> data)
    : m_tasks(data.size()),
      m_consumersCount(0),
      m_consumersStates(data.size(),
                        std::vector<StageTaskState>(maxConsumersCount,
                                                    StageTaskState::empty)),
      m_producingTask(nullptr),
      m_producingId(0),
      m_producersStates(data.size(), false),
      m_shutdownSignaled(false) {
  for (size_t i = 0; i < data.size(); ++i)
    m_tasks[i] = std::make_shared<StageTask<T>>(data[i]);
}

template <typename T>
SPMCStageConnection<T>::SPMCStageConnection(size_t connectionSize)
    : SPMCStageConnection(initData(connectionSize)) {}

template <typename T>
SPMCStageConnection<T>::~SPMCStageConnection() {
  shutdown();
}

template <typename T>
void SPMCStageConnection<T>::shutdown() {
  m_shutdownSignaled = true;
  m_waitConsumerTaskCv.notify_all();
  m_waitProducerTaskCv.notify_all();
}

template <typename T>
bool SPMCStageConnection<T>::isShutdown() const {
  return m_shutdownSignaled;
}

template <typename T>
std::shared_ptr<StageTask<T>> SPMCStageConnection<T>::getProducerTask() {
  std::unique_lock lock{m_mutex};

  auto taskIndex = findTaskIndexToProduce(lock);
  if (!taskIndex.has_value())
    return nullptr;
  auto index = taskIndex.value();

  for (size_t i = 0; i < m_consumersCount; ++i)
    m_consumersStates[index][i] = StageTaskState::empty;

  m_producersStates[index] = true;
  m_producingTask = m_tasks[index];
  m_producingId = index;

  return m_producingTask;
}

template <typename T>
void SPMCStageConnection<T>::taskProduced(std::shared_ptr<T> taskData,
                                           size_t taskId,
                                           bool produced) {
  std::unique_lock lock{m_mutex};

  size_t taskIndex = 0;

  if (m_producingTask && m_producingTask->data == taskData) {
    taskIndex = m_producingId;
  } else {
    for (size_t i = 0; i < m_tasks.size(); ++i) {
      if (m_tasks[i]->data == taskData) {
        taskIndex = i;
        break;
      }
    }
  }

  if (produced) {
    m_tasks[taskIndex]->taskId = taskId;
    setTaskState(taskIndex, StageTaskState::produced);
    m_waitConsumerTaskCv.notify_all();
  } else {
    m_tasks[taskIndex]->taskId = 0;
    setTaskState(taskIndex, StageTaskState::empty);
  }

  m_producersStates[taskIndex] = false;
}

template <typename T>
std::shared_ptr<StageTask<T>> SPMCStageConnection<T>::getConsumerTask(
    size_t consumerId,
    ConsumptionStrategy strategy,
    size_t minTaskId) {
  std::unique_lock lock{m_mutex};

  auto taskIndex =
      findTaskIndexToConsume(lock, consumerId, strategy, minTaskId);
  if (taskIndex == std::nullopt)
    return nullptr;
  auto index = taskIndex.value();

  m_consumersStates[index][consumerId] = StageTaskState::consumed;

  m_consumingTasks[consumerId] = m_tasks[index];
  m_consumingTasksId[consumerId] = index;

  return m_tasks[index];
}

template <typename T>
void SPMCStageConnection<T>::taskConsumed(std::shared_ptr<T> taskData,
                                           size_t consumerId,
                                           bool consumed) {
  if (consumerId >= m_consumersCount)
    throw std::invalid_argument(std::string("invalid consumerId:)") +
                                std::to_string(consumerId));
  {
    std::lock_guard lock{m_mutex};

    int taskId = -1;

    if (m_consumingTasks[consumerId] &&
        m_consumingTasks[consumerId]->data == taskData)
      taskId = m_consumingTasksId[consumerId];
    else {
      for (size_t i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i]->data == taskData) {
          taskId = i;
          break;
        }
      }
    }

    if (taskId < 0)
      throw PipelineException("invalid taskData");

    if (consumed)
      m_consumersStates[taskId][consumerId] = StageTaskState::empty;
  }

  m_waitProducerTaskCv.notify_one();
}

template <typename T>
size_t SPMCStageConnection<T>::connectConsumer() {
  if (m_consumersCount == maxConsumersCount)
    throw PipelineException(
        "Cannot connect consumer: the consumers' limit has been reached");

  auto consumerId = m_consumersCount;
  m_consumersCount++;

  m_consumingTasks[consumerId] = nullptr;
  m_consumingTasksId[consumerId] = 0;

  return consumerId;
}

template <typename T>
std::optional<size_t> SPMCStageConnection<T>::findTaskIndexToProduce(
    std::unique_lock<std::mutex>& lock) {
  std::optional<size_t> taskIndex = std::nullopt;

  auto taskId = std::numeric_limits<uint64_t>::max();
  while (!taskIndex.has_value() && !m_shutdownSignaled) {
    for (size_t i = 0; i < m_tasks.size(); ++i) {
      // find task with min taskIndex
      if (m_tasks[i]->taskId < taskId && !taskLocked(i)) {
        taskIndex = i;
        taskId = m_tasks[i]->taskId;
      }
    }

    if (!taskIndex.has_value())
      m_waitProducerTaskCv.wait_for(lock, std::chrono::milliseconds(100));
  }

  if (m_shutdownSignaled)
    return std::nullopt;

  return taskIndex;
}

template <typename T>
std::optional<size_t> SPMCStageConnection<T>::findTaskIndexToConsume(
    std::unique_lock<std::mutex>& lock,
    size_t consumerId,
    ConsumptionStrategy strategy,
    size_t minTaskId) {
  uint64_t taskId;
  if (strategy == ConsumptionStrategy::fifo)
    taskId = std::numeric_limits<uint64_t>::max();
  else if (strategy == ConsumptionStrategy::lifo)
    taskId = 0;

  std::optional<size_t> taskIndex = std::nullopt;

  while (taskIndex < 0 && !m_shutdownSignaled) {
    for (size_t i = 0; i < m_tasks.size(); ++i) {
      if (m_consumersStates[i][consumerId] == StageTaskState::produced &&
          m_tasks[i]->taskId > minTaskId) {
        if ((strategy == ConsumptionStrategy::lifo &&
             m_tasks[i]->taskId > taskId) ||
            (strategy == ConsumptionStrategy::fifo &&
             m_tasks[i]->taskId < taskId)) {
          taskIndex = i;
          taskId = m_tasks[i]->taskId;
        }
      }
    }

    if (taskIndex < 0)
      m_waitConsumerTaskCv.wait_for(lock, std::chrono::milliseconds(100));
  }

  return taskIndex;
}

template <typename T>
std::vector<std::shared_ptr<T>> SPMCStageConnection<T>::initData(size_t size) {
  std::vector<std::shared_ptr<T>> data(size);
  for (auto& el : data)
    el = std::make_shared<T>();

  return data;
}

template <typename T>
bool SPMCStageConnection<T>::taskLocked(size_t taskId) {
  if (m_producersStates[taskId])
    return true;

  for (auto i = 0u; i < m_consumersCount; i++) {
    auto& state = m_consumersStates[taskId][i];

    if (state == StageTaskState::consumed)
      return true;
  }

  return false;
}

template <typename T>
void SPMCStageConnection<T>::setTaskState(size_t taskId,
                                           StageTaskState state) {
  for (auto i = 0u; i < m_consumersCount; i++)
    m_consumersStates[taskId][i] = state;
}
