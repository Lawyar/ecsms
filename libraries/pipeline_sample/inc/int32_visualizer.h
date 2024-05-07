#pragma once

#include "consuming_stage.h"

namespace ecsms {
    class int32_visualizer : public consuming_stage<int32_t> {
    public:
        int32_visualizer(std::shared_ptr<mpmc_cycle_queue<int32_t>> connection);

        void consume(const int32_t&, bool& item_consumed) override;
    };
}
