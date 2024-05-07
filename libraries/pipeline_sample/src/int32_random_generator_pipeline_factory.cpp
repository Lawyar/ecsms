#include "int32_random_generator_pipeline_factory.h"
#include "int32_random_generator.h"
#include "int32_visualizer.h"

using namespace std;

namespace ecsms {
	shared_ptr<pipeline> int32_random_generator_pipeline_factory::create()
	{
		auto pipeline = make_shared<ecsms::pipeline>();

		auto connection = make_shared<mpmc_cycle_queue<int32_t>>(connection_capacity);
		shared_ptr<producing_stage<int32_t>> generator = make_shared<int32_random_generator>(connection);
		shared_ptr<consuming_stage<int32_t>> visuzlier = make_shared<int32_visualizer>(connection);

		pipeline->add(generator, visuzlier);

		return pipeline;
	}
}

