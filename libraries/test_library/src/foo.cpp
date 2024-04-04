#include "foo.h"

namespace ecsms {
    void foo::doFoo() {
        ++foos_;
    }

    uint64_t foo::getFoos() {
        return foos_;
    }
}
