#pragma once

#include "PipelineStage.h"

class PipelineStageNameConverter {
public:
  virtual ~PipelineStageNameConverter() = default;

  virtual int idByName(const std::string &) = 0;
  
  virtual std::string nameById(int id) = 0;
};
