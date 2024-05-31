#pragma once

#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineException.h"
#include "StageTaskState.h"

#include <atomic>
#include <optional>
#include <condition_variable>
#include <mutex>
#include <vector>

template <typename T>
class InOutStageConnection : public InStageConnection<T>,
                             public OutStageConnection<T> {
 public:
  InOutStageConnection(std::vector<std::shared_ptr<T>> data);

  InOutStageConnection(size_t connectionSize);

  ~InOutStageConnection() override;

  void shutdown() override;

  bool isShutdown() override;

  std::shared_ptr<StageTask<T>> getProducerTask() override;

  void releaseProducerTask(std::shared_ptr<T> taskData,
                           size_t taskId,
                           bool produced) override;

  std::shared_ptr<StageTask<T>> getConsumerTask(
      size_t consumerId,
      ConsumerStrategy strategy,
      size_t minTaskId) override;

  void releaseConsumerTask(std::shared_ptr<T> taskData,
                           size_t consumerId) override;

  size_t connectConsumer() override;

 private:
  void setTaskState(size_t taskId, StageTaskState);

  std::optional<size_t> findTaskIndexToProduce(std::unique_lock<std::mutex>& lock);

  std::optional<size_t> findTaskIndexToConsume(
      std::unique_lock<std::mutex>& lock,
      size_t consumerId,
      ConsumerStrategy strategy,
      size_t minTaskId);

  bool taskLocked(size_t taskId);

  static std::vector<std::shared_ptr<T>> initData(size_t size);

 private:
  static constexpr size_t maxConsumersCount = 32u;

  std::vector<std::shared_ptr<StageTask<T>>> m_tasks;
  std::vector<std::vector<StageTaskState>> m_tasksConsumingStates;
  std::vector<bool> m_tasksProducingStates;

  size_t m_consumersCount;

  std::shared_ptr<StageTask<T>> m_producingTask;
  size_t m_producingTaskId;

  std::shared_ptr<StageTask<T>> m_consumingTasks[maxConsumersCount];
  size_t m_consumingTasksId[maxConsumersCount];

  std::mutex m_mutex;

  std::condition_variable m_waitConsumerTaskCv;
  std::condition_variable m_waitProducerTaskCv;
  std::atomic_bool m_shutdownSignaled;
};

template <typename T>
InOutStageConnection<T>::InOutStageConnection(
    std::vector<std::shared_ptr<T>> data)
    : m_tasks(data.size()),
      m_tasksConsumingStates(
          data.size(),
          std::vector<StageTaskState>(maxConsumersCount, StageTaskState::empty)),
      m_tasksProducingStates(data.size(), false),
      m_consumersCount(0),
      m_producingTask(nullptr),
      m_producingTaskId(0),
      m_shutdownSignaled(false) {
  for (size_t i = 0; i < data.size(); ++i)
    m_tasks[i] = std::make_shared<StageTask<T>>(data[i]);
}

template <typename T>
InOutStageConnection<T>::InOutStageConnection(size_t connectionSize)
    : InOutStageConnection(initData(connectionSize)) {}

template <typename T>
InOutStageConnection<T>::~InOutStageConnection() {
  shutdown();
}

template <typename T>
void InOutStageConnection<T>::shutdown() {
  m_shutdownSignaled = true;
  m_waitConsumerTaskCv.notify_all();
  m_waitProducerTaskCv.notify_all();
}

template <typename T>
bool InOutStageConnection<T>::isShutdown() {
  return m_shutdownSignaled;
}

template <typename T>
std::shared_ptr<StageTask<T>> InOutStageConnection<T>::getProducerTask() {
  std::unique_lock lock{m_mutex};

  auto taskIndex = findTaskIndexToProduce(lock);
  if (!taskIndex.has_value())
    return nullptr;
  auto index = taskIndex.value();

  for (size_t i = 0; i < m_consumersCount; ++i)
    m_tasksConsumingStates[index][i] = StageTaskState::empty;

  m_tasksProducingStates[index] = true;
  m_producingTask = m_tasks[index];
  m_producingTaskId = index;

  return m_producingTask;
}

template <typename T>
void InOutStageConnection<T>::releaseProducerTask(std::shared_ptr<T> taskData,
                                                  size_t taskId,
                                                  bool produced) {
  std::unique_lock lock{m_mutex};

  size_t taskIndex = 0;

  if (m_producingTask && m_producingTask->data == taskData) {
    taskIndex = m_producingTaskId;
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
    setTaskState(taskIndex, StageTaskState::producing);

    m_waitConsumerTaskCv.notify_all();
  } else {
    m_tasks[taskIndex]->taskId = 0;
    setTaskState(taskIndex, StageTaskState::empty);
  }

  m_tasksProducingStates[taskIndex] = false;
}

template <typename T>
std::shared_ptr<StageTask<T>> InOutStageConnection<T>::getConsumerTask(
    size_t consumerId,
    ConsumerStrategy strategy,
    size_t minTaskId) {
  std::unique_lock lock{m_mutex};

  auto taskIndex = findTaskIndexToConsume(lock, consumerId, strategy, minTaskId);
  if (taskIndex == std::nullopt)
    return nullptr;
  auto index = taskIndex.value();

  m_tasksConsumingStates[index][consumerId] = StageTaskState::consuming;

  m_consumingTasks[consumerId] = m_tasks[index];
  m_consumingTasksId[consumerId] = index;

  return m_tasks[index];
}

template <typename T>
void InOutStageConnection<T>::releaseConsumerTask(std::shared_ptr<T> taskData,
                                                  size_t consumerId) {
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

    m_tasksConsumingStates[taskId][consumerId] = StageTaskState::empty;
  }

  m_waitProducerTaskCv.notify_one();
}

template <typename T>
size_t InOutStageConnection<T>::connectConsumer() {
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
std::optional<size_t> InOutStageConnection<T>::findTaskIndexToProduce(
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
std::optional<size_t> InOutStageConnection<T>::findTaskIndexToConsume(
    std::unique_lock<std::mutex>& lock,
    size_t consumerId,
    ConsumerStrategy strategy,
    size_t minTaskId) {
  uint64_t taskId;
  if (strategy == ConsumerStrategy::consumeOldest)
    taskId = 0;
  else if (strategy == ConsumerStrategy::consumeNewest)
    taskId = std::numeric_limits<uint64_t>::max();

  std::optional<size_t> taskIndex = std::nullopt;

  while (taskIndex < 0 && !m_shutdownSignaled) {
    for (size_t i = 0; i < m_tasks.size(); ++i) {
      if (m_tasksConsumingStates[i][consumerId] == StageTaskState::producing &&
          m_tasks[i]->taskId > minTaskId) {
        if ((strategy == ConsumerStrategy::consumeNewest &&
             m_tasks[i]->taskId > taskId) ||
            (strategy == ConsumerStrategy::consumeOldest &&
             m_tasks[i]->taskId < taskId)) {
          taskIndex = int(i);
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
std::vector<std::shared_ptr<T>> InOutStageConnection<T>::initData(size_t size) {
  std::vector<std::shared_ptr<T>> data(size);
  for (auto& el : data)
    el = std::make_shared<T>();

  return data;
}

template <typename T>
bool InOutStageConnection<T>::taskLocked(size_t taskId) {
  if (m_tasksProducingStates[taskId])
    return true;

  for (auto i = 0u; i < m_consumersCount; i++) {
    auto& state = m_tasksConsumingStates[taskId][i];

    if (state == StageTaskState::consuming)
      return true;
  }

  return false;
}

template <typename T>
void InOutStageConnection<T>::setTaskState(size_t taskId,
                                           StageTaskState state) {
  for (auto i = 0u; i < m_consumersCount; i++)
    m_tasksConsumingStates[taskId][i] = state;
}
