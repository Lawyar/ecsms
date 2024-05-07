#pragma once

#include <chrono>
#include <cstdint>

class SteadyClock {
public:
  static std::chrono::microseconds nowUs();
  static std::chrono::milliseconds nowMs();

  static void waitForUs(const std::chrono::milliseconds &);
  static void waitForMs(const std::chrono::microseconds &);
};
