#include "activeblockmodel.h"
#include "../events/changeactivenodeevent.h"

std::map<ConnectNodeWidget *, int> ActiveNodesModel::GetActiveNodes() {
  return _active_nodes;
}

void ActiveNodesModel::IncreaseNodeCount(ConnectNodeWidget *active_node) {
  if (active_node)
  {
    auto &&it = _active_nodes.find(active_node);
    if (it == _active_nodes.end()) {
      it = _active_nodes.emplace_hint(it, active_node, 0);
    }
    auto &&active_count = it->second;
    ++active_count;

    if (active_count == 1)
    {
      Notify(
          std::make_shared<ChangeActiveNodeEvent>(active_node, true));
    }
  }
}

void ActiveNodesModel::DecreaseNodeCount(ConnectNodeWidget *active_node) {
  if (active_node) {
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
}

int ActiveNodesModel::GetNodeCount(ConnectNodeWidget *active_node) {
  if (active_node) {
    return _active_nodes[active_node];
  }
  return 0;
}
