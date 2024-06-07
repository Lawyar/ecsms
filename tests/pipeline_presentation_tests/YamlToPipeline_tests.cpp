#include <gtest/gtest.h>

#include "YamlToPipeline.h"

using namespace std;

static constexpr auto PipelineYAML = "Int32RandomGenerator:\n"
"  id: \"Int 32 generator\"\n"
"  type: producer\n"
"\n"
"Int32Visualizer:\n"
"  id: \"Int 32 visualizer\"\n"
"  type: consumer\n"
"  parentStageId: \"Int 32 generator\"\n"
"\n"
"Int32ToDoubleConverter:\n"
"  id: \"Int 32 to double\"\n"
"  type: producerConsumer\n"
"  parentStageId: \"Int 32 generator\"\n"
"\n"
"DoubleVisualizer:\n"
"  id: \"Double visualizer\"\n"
"  type: consumer\n"
"  parentStageId: \"Int 32 to double\"\n";

TEST(YamlToPipeline_test, YamlToPipeline_loadWorks) {

}
