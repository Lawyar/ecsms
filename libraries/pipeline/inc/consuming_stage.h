#pragma once

#include "stage.h"
#include "mpmc_cycle_queue.h"

#include <iostream>

namespace ecsms {
    template<typename T>
    class consuming_stage : public stage {
    public:
        consuming_stage(std::shared_ptr<mpmc_cycle_queue<T>> connection);

        void run() override;

        virtual void consume(std::shared_ptr<T>, bool& item_consumed) = 0;

    private:
        std::shared_ptr<mpmc_cycle_queue<T>> connection_;
    };

    template<typename T>
    consuming_stage<T>::consuming_stage(std::shared_ptr<mpmc_cycle_queue<T>> connection)
        : connection_{connection}
    {
        if (!connection)
            throw std::invalid_argument("connection is null");
    }

    template<typename T>
    void consuming_stage<T>::run() {
        stage::run([this]() {
            std::shared_ptr<T> item;
            if (connection_->dequeue(item)) {
                bool consumed{};
                consume(item, consumed);
                if (!consumed)
                    connection_->enqueue(item);
            }
        });
    }
}
