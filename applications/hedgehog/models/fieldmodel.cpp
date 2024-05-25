#include "fieldmodel.h"
const QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>> &
FieldModel::GetConnectionMap() const {
  return _connection_map;
}

void FieldModel::Remove(ConnectNodeWidget *start) {
  _connection_map.remove(start);
  start->makeTransparent(true);
}

void FieldModel::AddConnection(ConnectNodeWidget *start,
                               ConnectNodeWidget *end) {
  _connection_map[start].append(end);
  start->makeTransparent(false);
  end->makeTransparent(false);
}

void FieldModel::RemoveConnection(ConnectNodeWidget *start,
                                  ConnectNodeWidget *end) {
  _connection_map[start].erase(std::find(_connection_map[start].begin(),
                                         _connection_map[start].end(), end));
  if (_connection_map[start].empty())
    _connection_map.remove(start);

  for (auto && node : {start, end})
    if (!IsNodeUsed(node))
      node->makeTransparent(true);
}

bool FieldModel::IsNodeUsed(ConnectNodeWidget *node) const {
  for (auto iter = _connection_map.begin(); iter != _connection_map.end();
       ++iter) {
    auto && current_node = iter.key();
    auto && current_heighbor_nodes = iter.value();
    if (node == current_node)
      return true;
    if (current_heighbor_nodes.contains(node))
      return true;
  }
  return false;
}
