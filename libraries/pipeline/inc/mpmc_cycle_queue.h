#include <cds/container/vyukov_mpmc_cycle_queue.h>

#include <memory>

namespace ecsms {
	template<typename T>
	using mpmc_cycle_queue = cds::container::VyukovMPMCCycleQueue<std::shared_ptr<T>>;
}
