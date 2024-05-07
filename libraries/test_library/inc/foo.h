#pragma once

#include <cstdint>

namespace ecsms {
class foo {
public:
  void doFoo();

  uint64_t getFoos();

private:
  uint64_t foos_ = 0;
};
} // namespace ecsms
