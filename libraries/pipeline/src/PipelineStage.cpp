#include "PipelineStage.h"

#include <iostream>

using namespace std;

PipelineStage::PipelineStage(const string_view stageName)
    : m_stageName(stageName), m_shutdownSignaled(false) {}

PipelineStage::~PipelineStage() { stop(); }

void PipelineStage::stop() {
  m_shutdownSignaled = true;

  if (m_thread.joinable())
    m_thread.join();
}

string PipelineStage::getName() { return m_stageName; }

void PipelineStage::set(std::any) {}
