#pragma once

#include "producing_stage.h"
#include "consuming_stage.h"

#include <memory>

namespace ecsms {
    class pipeline {
    public:
        template<typename T>
        void add(std::shared_ptr<producing_stage<T>>, std::shared_ptr<consuming_stage<T>>);

        template<typename T>
        void add(std::shared_ptr<producing_stage<T>>);
    private:
        std::vector<std::shared_ptr<stage>> stages_;
    };

    template<typename T>
    void pipeline::add(std::shared_ptr<producing_stage<T>> producer, std::shared_ptr<consuming_stage<T>> consumer) {
        stages_.push_back(producer);
        stages_.push_back(consumer);
    }

    template<typename T>
    void pipeline::add(std::shared_ptr<producing_stage<T>> producer) {
        stages_.push_back(producer);
    }
}
