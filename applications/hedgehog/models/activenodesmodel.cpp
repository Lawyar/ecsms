#include "activenodesmodel.h"
#include "../events/changeactivenodeevent.h"

std::map<NodeId, int> ActiveNodesModel::GetActiveNodes() {
  return _active_nodes;
}

void ActiveNodesModel::IncreaseNodeCount(const NodeId &active_node) {
  auto &&it = _active_nodes.find(active_node);
  if (it == _active_nodes.end()) {
    it = _active_nodes.emplace_hint(it, active_node, 0);
  }
  auto &&active_count = it->second;
  ++active_count;

  if (active_count == 1) {
    Notify(std::make_shared<ChangeActiveNodeEvent>(active_node, true));
  }
}

void ActiveNodesModel::DecreaseNodeCount(const NodeId &active_node) {
  auto &&it = _active_nodes.find(active_node);
  if (it == _active_nodes.end()) {
    assert(false);
    return;
  }
  auto &&active_count = it->second;
  --active_count;

  if (active_count <= 0) {
    assert(active_count == 0);
    Notify(std::make_shared<ChangeActiveNodeEvent>(active_node, false));
  }
}

int ActiveNodesModel::GetNodeCount(const NodeId &active_node) {
  return _active_nodes[active_node];
}

std::optional<NodeId> ActiveNodesModel::GetBeginOfLine() const {
  return _begin;
}

void ActiveNodesModel::SetBeginOfLine(std::optional<NodeId> begin) {
  _begin = begin;
}
