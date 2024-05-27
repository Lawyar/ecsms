#pragma once

#include "../connectnodewidget.h"
#include "imodel.h"

#include <map>
#include <optional>

class ActiveNodesModel : public IModel {
public:
  ActiveNodesModel() = default;
  std::map<NodeId, int> GetActiveNodes();
  void IncreaseNodeCount(const NodeId &active_node);
  void DecreaseNodeCount(const NodeId &active_node);
  int GetNodeCount(const NodeId &active_node);
  std::optional<NodeId> GetBeginOfLine() const;
  void SetBeginOfLine(std::optional<NodeId> begin);

private:
  std::map<NodeId, int> _active_nodes;
  std::optional<NodeId> _begin;
};
