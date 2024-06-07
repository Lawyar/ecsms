#pragma once

#include "InOutStageConnection.h"
#include "Pipeline.h"

#include <memory>

class Int32RandomGeneratorPipelineFactory {
public:
  static std::shared_ptr<Pipeline> create();

private:
  static constexpr size_t connectionCapacity = 10;
};
