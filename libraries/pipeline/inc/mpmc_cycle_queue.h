#include "cds/container/vyukov_mpmc_cycle_queue.h"

namespace ecsms {
	template<typename T>
	using mpmc_cycle_queue = VyukovMPMCCycleQueue<T>;
}
