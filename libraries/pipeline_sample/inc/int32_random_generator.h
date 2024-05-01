#pragma once

#include "producing_stage.h"

namespace ecsms {
	class int32_random_generator : public producing_stage<int32_t> {
	public:
		int32_random_generator(std::shared_ptr<mpmc_cycle_queue<int32_t>> connection);

		std::shared_ptr<int32_t> produce(bool& item_produced) override;
	};
}
