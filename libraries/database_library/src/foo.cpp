#include "foo.h"

namespace ecsms {
    void bar::doBar() {
        ++bars_;
    }

    uint64_t bar::getBars() {
        return bars_;
    }
}
