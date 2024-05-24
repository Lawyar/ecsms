#pragma once

#include "InOutStageConnection.h"
#include "Pipeline.h"

#include <memory>

class int32_random_generator_pipeline_factory {
public:
  static std::shared_ptr<Pipeline> create();

private:
  static std::shared_ptr<InOutStageConnection<int32_t>> createConnection();

  static constexpr size_t connection_capacity = 10;
};
