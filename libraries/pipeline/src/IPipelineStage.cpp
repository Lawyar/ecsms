#include "IPipelineStage.h"

#include <iostream>

using namespace std;

IPipelineStage::IPipelineStage(const string_view stageName)
    : m_stageName(stageName) {}

string IPipelineStage::getName() {
  return m_stageName;
}

void IPipelineStage::set(std::any) {}
