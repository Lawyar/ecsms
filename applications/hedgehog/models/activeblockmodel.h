#pragma once

#include "../connectnodewidget.h"
#include "imodel.h"

#include <map>

class ActiveNodesModel : public IModel {
public:
  ActiveNodesModel() = default;
  std::map<ConnectNodeWidget *, int> GetActiveNodes();
  void IncreaseNodeCount(ConnectNodeWidget *active_node);
  void DecreaseNodeCount(ConnectNodeWidget *active_node);
  int GetNodeCount(ConnectNodeWidget *active_node);

private:
  std::map<ConnectNodeWidget *, int> _active_nodes;
};
