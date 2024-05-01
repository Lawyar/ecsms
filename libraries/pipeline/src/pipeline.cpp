#include "pipeline.h"

namespace ecsms {
	void pipeline::run() {
		if (is_running_)
			throw std::runtime_error("pipeline has already been running");
		for (auto& stage : stages_) {
			stage->run();
		}
		is_running_ = true;
	}
}
