#pragma once
#include "../connectnodewidget.h"
#include "../models/activeblockmodel.h"

class ActiveNodesLock {
public:
  ActiveNodesLock(ActiveNodesModel &active_nodes_model, const std::vector<ConnectNodeWidget *> & nodes);
  ~ActiveNodesLock();

private:
  ActiveNodesModel &_active_nodes_model;
  std::vector<ConnectNodeWidget *> _nodes;
};
