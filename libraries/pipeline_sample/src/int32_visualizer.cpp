#include "int32_visualizer.h"

#include <iostream>

using namespace std;

namespace ecsms {
    int32_visualizer::int32_visualizer(shared_ptr<mpmc_cycle_queue<int32_t>> connection)
        : consuming_stage(connection)
    {}

    void int32_visualizer::consume(std::shared_ptr<int32_t> item, bool& item_consumed) {
        //if(item)
        //    cout << *item << endl;
        item_consumed = true;
    }
}
