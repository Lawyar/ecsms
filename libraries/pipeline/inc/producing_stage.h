#pragma once

#include "stage.h"
#include "mpmc_cycle_queue.h"

namespace ecsms {
    template<typename T>
    class producing_stage : public stage {
    public:
        producing_stage(std::shared_ptr<mpmc_cycle_queue<T>> connection);

        void run() override;

        virtual T produce(bool& item_produced) = 0;

    private:
        std::shared_ptr<mpmc_cycle_queue<T>> connection_;
    };

    template<typename T>
    producing_stage<T>::producing_stage(std::shared_ptr<mpmc_cycle_queue<T>> connection)
        : connection_{ connection }
    {
        if (!connection)
            throw std::invalid_argument("connection is null");
    }

    template<typename T>
    void producing_stage<T>::run() {
        stage::start_thread([&](){
            bool produced{};
            auto item = produce(produced);
            if (produced)
                connection_->enqueue(std::move(item));
        });
    }
}
