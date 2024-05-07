#include "SteadyClock.h"

#include <thread>

using namespace std;

chrono::microseconds SteadyClock::nowUs() {
  auto now = std::chrono::steady_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(now);
}

chrono::milliseconds SteadyClock::nowMs() {
  auto now = std::chrono::steady_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now);
}

void SteadyClock::waitForUs(const std::chrono::milliseconds &t) {
  this_thread::sleep_for(t);
}

void SteadyClock::waitForMs(const std::chrono::microseconds &t) {
  this_thread::sleep_for(t);
}
