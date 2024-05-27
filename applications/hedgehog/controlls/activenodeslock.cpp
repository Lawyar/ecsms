#include "activenodeslock.h"

ActiveNodesLock::ActiveNodesLock(ActiveNodesModel &active_nodes_model,
                                 const std::vector<NodeId> &nodes)
    : _active_nodes_model(active_nodes_model), _nodes(nodes) {
  for (auto &&node : _nodes)
    _active_nodes_model.IncreaseNodeCount(node);
}

ActiveNodesLock::~ActiveNodesLock() {
  for (auto && node : _nodes)
    _active_nodes_model.DecreaseNodeCount(node);
}
