#pragma once
#include "../connectnodewidget.h"
#include "../models/fieldmodel.h"

#include <functional>

class ActiveNodesLock {
public:
  using Functor = std::function<bool(const NodeId &)>;

public:
  ActiveNodesLock(FieldModel &field_model, const std::vector<NodeId> &nodes,
                  Functor &&is_node_used_func);
  ~ActiveNodesLock();

private:
  FieldModel &_field_model;
  std::vector<NodeId> _nodes;
  Functor _is_node_used_func;
};
