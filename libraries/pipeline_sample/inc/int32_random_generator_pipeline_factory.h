#pragma once

#include "pipeline.h"

#include <memory>

namespace ecsms {
	class int32_random_generator_pipeline_factory {
	public:
		static std::shared_ptr<pipeline> create();

	private:
		static constexpr size_t connection_capacity = 10;
	};
}
