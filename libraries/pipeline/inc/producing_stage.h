#pragma once

#include "stage.h"
#include "mpmc_cycle_queue.h"

namespace ecsms {
    template<typename T>
    class producing_stage : public stage {
    public:
        producing_stage(std::shared_ptr<mpmc_cycle_queue> connection);

        virtual ~producing_stage() = default;

        virtual T produce() = 0;

    private:
        std::shared_ptr<mpmc_cycle_queue> connection_;
    };

    template<typename T>
    producing_stage<T>::producing_stage(std::shared_ptr<mpmc_cycle_queue<T>> connection)
        : connection_{ connection }
    {
        if (!connection)
            throw std::invalid_argument("connection is null");
    }
}
