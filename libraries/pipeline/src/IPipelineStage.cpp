#include "IPipelineStage.h"

#include <iostream>

using namespace std;

IPipelineStage::IPipelineStage(const string_view stageName)
    : m_stageName(stageName), m_id(nullopt), m_parentId(nullopt) {}

string IPipelineStage::getName() const {
  return m_stageName;
}

void IPipelineStage::setId(const string_view id) {
  m_id = id;
}

optional<string> IPipelineStage::getId() const {
  return m_id;
}

void IPipelineStage::setParentId(const string_view id) {
  m_parentId = id;
}

optional<string> IPipelineStage::getParentId() const {
  return m_parentId;
}
