#include <gtest/gtest.h>

#include "foo.h"

using namespace ecsms;

TEST(test_library_tests, doFoo_valid) {
    foo f;

    f.doFoo();
    f.doFoo();

    ASSERT_EQ(f.getFoos(), 2);
}
