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
        template<typename F, class... Args>
        void run(F&& function, Args&&... args);

    private:
        std::thread thread_;
        std::atomic_bool shutdown_signaled_;
    };

    template<typename F, class... Args>
    void stage::run(F&& function, Args&&... args) {
        thread_ = std::thread([&]() {
            while(!shutdown_signaled_) {
                std::invoke(std::forward<F>(function), std::forward<Args>(args)...);
            }
        });
    }
}
