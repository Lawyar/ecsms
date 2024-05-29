#include "activenodeslock.h"
#include "../events/changeactivenodeevent.h"

ActiveNodesLock::ActiveNodesLock(FieldModel &field_model,
                                 const std::vector<NodeId> &nodes,
                                 Functor && is_node_used_func)
    : _field_model(field_model), _nodes(nodes), _is_node_used_func(std::move(is_node_used_func)) {
  for (auto &&node : _nodes)
    _field_model.Notify(std::make_shared<ChangeActiveNodeEvent>(node, true));
}


ActiveNodesLock::~ActiveNodesLock() {
  for (auto &&node : _nodes)
    if (!_is_node_used_func(node))
      _field_model.Notify(std::make_shared<ChangeActiveNodeEvent>(node, false));
}

const std::vector<NodeId> &ActiveNodesLock::GetLockedNodes() const {
  return _nodes;
}
