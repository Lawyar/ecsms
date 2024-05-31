#pragma once

#include <cstdint>
#include <memory>

template <typename T>
struct StageTask {
  StageTask(std::shared_ptr<T> d) : data{d}, taskId{} {}

  std::shared_ptr<T> data;
  size_t taskId;
};
