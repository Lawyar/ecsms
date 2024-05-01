#include "int32_random_generator_pipeline_factory.h"
#include "int32_random_generator.h"
#include "int32_visualizer.h"

using namespace ecsms;
using namespace std;

int main() {
	auto pipeline = int32_random_generator_pipeline_factory::create();

	auto connection = make_shared<mpmc_cycle_queue<int32_t>>(512);
	shared_ptr<producing_stage<int32_t>> generator = make_shared<int32_random_generator>(connection);
	auto visuzlier = make_shared<int32_visualizer>(connection);

	generator->run();
	visuzlier->run();

	while (true) {}

}