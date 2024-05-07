#pragma once

#include "ProducerStage.h"

class int32_random_generator : public ProducerStage<int32_t> {
public:
	int32_random_generator(std::shared_ptr<mpmc_cycle_queue<int32_t>> connection);

	int32_t produce(bool& item_produced) override;
};
