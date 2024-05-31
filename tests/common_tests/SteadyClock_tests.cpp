#include <gtest/gtest.h>

#include "SteadyClock.h"

using namespace std;
using namespace chrono;

TEST(SteadyClock_tests, nowUsDoesntThrow) {
  ASSERT_NO_THROW(SteadyClock::nowUs());
}

TEST(SteadyClock_tests, nowMsDoesntThrow) {
  ASSERT_NO_THROW(SteadyClock::nowMs());
}

TEST(SteadyClock_tests, waitForUsDoesntThrow) {
  ASSERT_NO_THROW(SteadyClock::waitForUs(1000));
}

TEST(SteadyClock_tests, waitForMsDoesntThrow) {
  ASSERT_NO_THROW(SteadyClock::waitForMs(1));
}
