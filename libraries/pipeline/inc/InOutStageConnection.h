#pragma once

#include "InStageConnection.h"
#include "OutStageConnection.h"
#include "PipelineException.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <vector>

template <typename T>
class InOutStageConnection : public InStageConnection<T>,
                             public OutStageConnection<T> {
public:
  InOutStageConnection(std::vector<std::shared_ptr<T>> data);

  InOutStageConnection(size_t connectionSize);

  void shutdown() override;

  std::shared_ptr<StageTask<T>> getProducerTask() override;
  void releaseProducerTask(std::shared_ptr<T> taskData, uint64_t timestamp,
                           bool produced) override;

  std::shared_ptr<StageTask<T>>
  getConsumerTask(size_t consumerId, TaskRetrieveStrategy strategy,
                  uint64_t leastTimestamp) override;
  void releaseConsumerTask(std::shared_ptr<T> taskData,
                           size_t consumerId) override;

  bool consumerTasksAvailable(size_t consumerId,
                              uint64_t leastTimestamp) const override;

  template<typename StageT> static bool connectable() {
    return is_same_v<StageT, T>;
  }

private:
  size_t onConsumerConnected() override;

  void onProducerConnected() override;

  static std::vector<std::shared_ptr<T>> initData(size_t size);

private:
  enum TaskState { empty, produced, consuming };

  using InStageConnection<T>::max_consumers_count;
  using TaskStates = std::vector<TaskState>;

private:
  bool taskBusy(size_t taskId);
  void setTaskState(size_t taskId, TaskState);

private:
  const size_t m_tasksCount;

  std::vector<std::shared_ptr<StageTask<T>>> m_tasks;
  std::vector<TaskStates> m_tasksConsumingStates;
  std::vector<bool> m_tasksProducingStates;

  size_t m_consumersCount;

  std::shared_ptr<StageTask<T>> m_producingTask;
  size_t m_producingTaskId;

  std::shared_ptr<StageTask<T>> m_consumingTasks[max_consumers_count];
  size_t m_consumingTasksId[max_consumers_count];

  mutable std::mutex m_mutex;

  std::condition_variable m_waitConsumerTaskCv;
  std::condition_variable m_waitProducerTaskCv;
  std::atomic_bool m_shutdownSignaled;
};

template <typename T>
InOutStageConnection<T>::InOutStageConnection(
    std::vector<std::shared_ptr<T>> data)
    : m_tasksCount(data.size()), m_tasks(data.size()),
      m_tasksConsumingStates(data.size()), m_tasksProducingStates(data.size()),
      m_consumersCount(0), m_producingTask(nullptr), m_producingTaskId(0),
      m_shutdownSignaled(false) {
  for (auto i = 0u; i < m_tasksCount; i++) {
    m_tasks[i] = std::make_shared<StageTask<T>>(data[i]);
    m_tasksConsumingStates[i] = TaskStates(max_consumers_count, empty);
    m_tasksProducingStates[i] = false;
  }
}

template <typename T>
InOutStageConnection<T>::InOutStageConnection(size_t connectionSize)
    : InOutStageConnection(initData(connectionSize)) {}

template <typename T> void InOutStageConnection<T>::shutdown() {
  m_shutdownSignaled = true;
  m_waitConsumerTaskCv.notify_all();
  m_waitProducerTaskCv.notify_all();
}

template <typename T>
std::shared_ptr<StageTask<T>> InOutStageConnection<T>::getProducerTask() {
  int id = -1;
  auto timestamp = std::numeric_limits<uint64_t>::max();

  std::unique_lock lock{m_mutex};

  while (id < 0 && !m_shutdownSignaled) {
    for (auto i = 0u; i < m_tasksCount; i++) {
      if (m_tasks[i]->timestamp < timestamp && !taskBusy(i)) {
        id = int(i);
        timestamp = m_tasks[i]->timestamp;
      }
    }

    if (id < 0)
      m_waitProducerTaskCv.wait_for(lock, std::chrono::milliseconds(100));
  }

  if (m_shutdownSignaled)
    return nullptr;

  auto &states = m_tasksConsumingStates[id];
  for (auto i = 0u; i < m_consumersCount; i++)
    states[i] = empty;

  m_tasksProducingStates[id] = true;
  m_producingTask = m_tasks[id];
  m_producingTaskId = id;

  return m_producingTask;
}

template <typename T>
void InOutStageConnection<T>::releaseProducerTask(std::shared_ptr<T> taskData,
                                                  uint64_t timestamp,
                                                  bool produced) {
  {
    std::unique_lock lock{m_mutex};

    int taskId = 0;

    if (m_producingTask && m_producingTask->data == taskData) {
      taskId = m_producingTaskId;
    } else {
      for (auto i = 0u; i < m_tasksCount; i++) {
        if (m_tasks[i]->data == taskData) {
          taskId = int(i);
          break;
        }
      }
    }

    if (taskId < 0)
      throw PipelineException("invalid taskData");

    if (produced) {
      m_tasks[taskId]->timestamp = timestamp;
      setTaskState(taskId, TaskState::produced);

      m_waitConsumerTaskCv.notify_all();
    } else {
      m_tasks[taskId]->timestamp = 0u;
      setTaskState(taskId, TaskState::empty);
    }

    m_tasksProducingStates[taskId] = false;
  }
}

template <typename T>
std::shared_ptr<StageTask<T>> InOutStageConnection<T>::getConsumerTask(
    size_t consumerId, TaskRetrieveStrategy strategy, uint64_t leastTimestamp) {
  if (consumerId >= m_consumersCount)
    throw std::invalid_argument(std::string("invalid consumerId:)") +
                                std::to_string(consumerId));

  uint64_t timestamp;
  if (strategy == TaskRetrieveStrategy::newest)
    timestamp = 0;
  else if (strategy == TaskRetrieveStrategy::oldest)
    timestamp = std::numeric_limits<uint64_t>::max();
  else
    throw std::invalid_argument("unknown strategy");

  std::unique_lock lock{m_mutex};

  int id = -1;
  while (id < 0 && !m_shutdownSignaled) {
    for (auto i = 0u; i < m_tasksCount; i++) {
      if (m_tasksConsumingStates[i][consumerId] == produced &&
          m_tasks[i]->timestamp > leastTimestamp) {
        if ((strategy == TaskRetrieveStrategy::newest &&
             m_tasks[i]->timestamp > timestamp) ||
            (strategy == TaskRetrieveStrategy::oldest &&
             m_tasks[i]->timestamp < timestamp)) {
          id = int(i);
          timestamp = m_tasks[i]->timestamp;
        }
      }
    }

    if (id < 0)
      m_waitConsumerTaskCv.wait_for(lock, std::chrono::milliseconds(100));
  }

  if (m_shutdownSignaled)
    return nullptr;

  m_tasksConsumingStates[id][consumerId] = consuming;

  m_consumingTasks[consumerId] = m_tasks[id];
  m_consumingTasksId[consumerId] = id;

  return m_tasks[id];
}

template <typename T>
void InOutStageConnection<T>::releaseConsumerTask(std::shared_ptr<T> taskData,
                                                  size_t consumerId) {
  {
    if (consumerId >= m_consumersCount)
      throw std::invalid_argument(std::string("invalid consumerId:)") +
                                  std::to_string(consumerId));

    int taskId = -1;

    std::unique_lock lock{m_mutex};

    if (m_consumingTasks[consumerId] &&
        m_consumingTasks[consumerId]->data == taskData)
      taskId = m_consumingTasksId[consumerId];
    else {
      for (auto i = 0u; i < m_tasksCount; i++) {
        if (m_tasks[i]->data == taskData) {
          taskId = i;
          break;
        }
      }
    }

    if (taskId < 0)
      throw PipelineException("invalid taskData");

    m_tasksConsumingStates[taskId][consumerId] = empty;
  }

  m_waitProducerTaskCv.notify_all();
}

template <typename T>
bool InOutStageConnection<T>::consumerTasksAvailable(
    size_t consumerId, uint64_t leastTimestamp) const {
  if (consumerId >= m_consumersCount)
    throw std::invalid_argument(std::string("invalid consumerId: ") +
                                std::to_string(consumerId));

  std::unique_lock lock{m_mutex};

  for (auto i = 0u; i < m_tasksCount; i++)
    if (m_tasks[i]->timestamp > leastTimestamp &&
        m_tasksConsumingStates[i][consumerId] == produced)
      return true;

  return false;
}

template <typename T> size_t InOutStageConnection<T>::onConsumerConnected() {
  if (m_consumersCount == max_consumers_count)
    throw PipelineException(
        "Cannot connect consumer: the consumers' limit has been reached");

  auto consumerId = m_consumersCount;
  m_consumersCount++;

  m_consumingTasks[consumerId] = nullptr;
  m_consumingTasksId[consumerId] = 0;

  return consumerId;
}

template <typename T> void InOutStageConnection<T>::onProducerConnected() {}

template <typename T>
std::vector<std::shared_ptr<T>> InOutStageConnection<T>::initData(size_t size) {
  std::vector<std::shared_ptr<T>> data(size);
  for (auto &el : data)
    el = std::make_shared<T>();

  return data;
}

template <typename T> bool InOutStageConnection<T>::taskBusy(size_t taskId) {
  if (m_tasksProducingStates[taskId])
    return true;

  for (auto i = 0u; i < m_consumersCount; i++) {
    auto &state = m_tasksConsumingStates[taskId][i];

    if (state == consuming)
      return true;
  }

  return false;
}

template <typename T>
void InOutStageConnection<T>::setTaskState(size_t taskId, TaskState state) {
  for (auto i = 0u; i < m_consumersCount; i++)
    m_tasksConsumingStates[taskId][i] = state;
}
