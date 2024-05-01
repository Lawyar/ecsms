#pragma once

#include "producing_stage.h"
#include "consuming_stage.h"

#include <memory>

namespace ecsms {
    class pipeline {
    public:
        template<typename T>
        void add(std::shared_ptr<producing_stage<T>>, std::shared_ptr<consuming_stage<T>>);

        void run();

        // template<typename T>
        // void add(std::shared_ptr<producing_stage<T>>);
    private:
        std::vector<std::shared_ptr<stage>> stages_;
        bool is_running_ = false;
    };

    template<typename T>
    void pipeline::add(std::shared_ptr<producing_stage<T>> producer, std::shared_ptr<consuming_stage<T>> consumer) {
        if (is_running_)
            throw std::runtime_error("error on adding stages: pipeline has already been running");
        stages_.push_back(producer);
        stages_.push_back(consumer);
    }
}
