#include "activenodesmodel.h"
#include "../events/changeactivenodeevent.h"
#include "../utility/containerutility.h"

#include <QMap>

std::map<NodeId, int> ActiveNodesModel::GetActiveNodes() {
  return _active_nodes;
}

void ActiveNodesModel::IncreaseNodeCount(const NodeId &active_node, int count) {
  auto &&it = _active_nodes.find(active_node);
  if (it == _active_nodes.end()) {
    it = _active_nodes.emplace_hint(it, active_node, 0);
  }
  auto &&active_count = it->second;
  active_count += count;

  if (active_count == 1) {
    Notify(std::make_shared<ChangeActiveNodeEvent>(active_node, true));
  }
}

void ActiveNodesModel::DecreaseNodeCount(const NodeId &active_node, int count) {
  auto &&it = _active_nodes.find(active_node);
  if (it == _active_nodes.end()) {
    assert(false);
    return;
  }
  auto &&active_count = it->second;
  active_count -= count;

  if (active_count <= 0) {
    assert(active_count == 0);
    Notify(std::make_shared<ChangeActiveNodeEvent>(active_node, false));
    _active_nodes.erase(active_node);
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

ActiveNodesModel::Memento ActiveNodesModel::Save() const {
  Memento res{_active_nodes, _begin};
  return res;
}

void ActiveNodesModel::Load(const ActiveNodesModel::Memento &m) {
  auto &&node_to_activate =
      SubstractionKeys(QMap(m._active_nodes), QMap(_active_nodes));
  auto &&node_to_deactivate =
      SubstractionKeys(QMap(_active_nodes), QMap(m._active_nodes));
  
  _active_nodes = m._active_nodes;
  _begin = m._begin;

  for (auto &&id : node_to_activate) {
    Notify(std::make_shared<ChangeActiveNodeEvent>(id, true));
  }

  for (auto &&id : node_to_deactivate) {
    Notify(std::make_shared<ChangeActiveNodeEvent>(id, false));
  }
}
