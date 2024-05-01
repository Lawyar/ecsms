#pragma once

#include <cstdint>

namespace ecsms {
    class bar {
    public:
        void doBar();

        uint64_t getBars();
    private:
        uint64_t bars_ = 0;
    };
}
