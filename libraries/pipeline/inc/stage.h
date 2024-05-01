#pragma once

#include <atomic>
#include <thread>
#include <memory>
#include <vector>

namespace ecsms {
    class stage {
    public:
        virtual ~stage();

        virtual void run() = 0;

        void shutdown();

    protected:
        template<typename F>
        void start_thread(F&& function);

    protected:
        std::thread thread_;
        std::atomic_bool shutdown_signaled_;
    };

    template<typename F>
    void stage::start_thread(F&& function) {
        thread_ = std::thread([&]() {
            while(!shutdown_signaled_) {
                function();
            }
        });
    }
}
