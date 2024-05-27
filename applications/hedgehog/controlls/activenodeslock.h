#pragma once
#include "../connectnodewidget.h"
#include "../models/activenodesmodel.h"

class ActiveNodesLock {
public:
  ActiveNodesLock(ActiveNodesModel &active_nodes_model,
                  const std::vector<NodeId> &nodes);
  ~ActiveNodesLock();

private:
  ActiveNodesModel &_active_nodes_model;
  std::vector<NodeId> _nodes;
};
