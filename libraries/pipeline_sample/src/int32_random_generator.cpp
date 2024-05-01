#include "int32_random_generator.h"

using namespace std;

namespace ecsms {
	int32_random_generator::int32_random_generator(shared_ptr<mpmc_cycle_queue<int32_t>> connection)
		: producing_stage(connection)
	{
		srand(time(nullptr));
	}

	int32_t ecsms::int32_random_generator::produce(bool& item_produced) {
		this_thread::sleep_for(chrono::milliseconds(500));
		item_produced = true;
		return rand() % 100;
	}
}
