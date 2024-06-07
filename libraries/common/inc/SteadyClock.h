#pragma once

#include <chrono>
#include <cstdint>

class SteadyClock {
 public:
  static std::chrono::microseconds nowUs();
  static std::chrono::milliseconds nowMs();

  static void waitForUs(unsigned);
  static void waitForMs(unsigned);
  static void waitForUs(const std::chrono::microseconds&);
  static void waitForMs(const std::chrono::milliseconds&);
};
