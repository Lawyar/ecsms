#pragma once

#include <cstdint>
#include <memory>

template <typename T> struct StageTask {
  StageTask(std::shared_ptr<T> d) : data(d), timestamp(0) {}

  std::shared_ptr<T> data;
  uint64_t timestamp;
};
