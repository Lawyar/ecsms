#pragma once

#include <thread>
#include <memory>
#include <vector>

namespace ecsms {
    class stage {
    public:
        virtual ~stage();

        virtual void run() = 0;

    protected:
        template<typename F>
        void run(F&& function);

    private:
        std::thread _thread;
    };

    template<typename F>
    void stage::run(F&& function) {
        _thread = std::thread(function);
    }
}
