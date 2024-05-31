#include "SteadyClock.h"

#include <thread>

using namespace std;

chrono::microseconds SteadyClock::nowUs() {
  auto now = chrono::steady_clock::now().time_since_epoch();
  return chrono::duration_cast<chrono::microseconds>(now);
}

chrono::milliseconds SteadyClock::nowMs() {
  auto now = chrono::steady_clock::now().time_since_epoch();
  return chrono::duration_cast<chrono::milliseconds>(now);
}

void SteadyClock::waitForUs(unsigned t) {
  waitForUs(chrono::microseconds(t));
}

void SteadyClock::waitForMs(unsigned t) {
  waitForUs(chrono::milliseconds(t));
}

void SteadyClock::waitForUs(const chrono::microseconds& t) {
  this_thread::sleep_for(t);
}

void SteadyClock::waitForMs(const chrono::milliseconds& t) {
  this_thread::sleep_for(t);
}
