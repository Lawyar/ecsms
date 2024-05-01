#pragma once

#include "stage.h"
#include "mpmc_cycle_queue.h"

namespace ecsms {
    template<typename T>
    class consuming_stage : public stage {
    public:
        consuming_stage(std::shared_ptr<mpmc_cycle_queue> connection);

        virtual ~consuming_stage() = default;

        virtual void consume(T) = 0;

    private:
        std::shared_ptr<mpmc_cycle_queue> connection_;
    };

    template<typename T>
    consuming_stage<T>::consuming_stage(std::shared_ptr<mpmc_cycle_queue<T>> connection)
        : connection_{connection}
    {
        if (!connection)
            throw std::invalid_argument("connection is null");
    }
}
