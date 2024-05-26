#include "fieldmodel.h"

const QMap<NodeId, QVector<NodeId>> &FieldModel::GetConnectionMap() const {
  return _connection_map;
}

void FieldModel::Remove(const NodeId &start) { _connection_map.remove(start); }

void FieldModel::AddConnection(const NodeId &start, const NodeId &end) {
  _connection_map[start].append(end);
}

void FieldModel::RemoveConnection(const NodeId &start, const NodeId &end) {
  _connection_map[start].erase(std::find(_connection_map[start].begin(),
                                         _connection_map[start].end(), end));
  if (_connection_map[start].empty())
    _connection_map.remove(start);
}

bool FieldModel::IsNodeUsed(const NodeId &node) const {
  for (auto iter = _connection_map.begin(); iter != _connection_map.end();
       ++iter) {
    auto &&current_node = iter.key();
    auto &&current_heighbor_nodes = iter.value();
    if (node == current_node)
      return true;
    if (current_heighbor_nodes.contains(node))
      return true;
  }
  return false;
}

void FieldModel::AddBlock(const BlockId &block, const BlockData &bd) {
  _blocks[block] = bd;
}

void FieldModel::RemoveBlock(const BlockId &block) {
  for (auto &&node : {block.GetNodeId(Incoming), block.GetNodeId(Outgoing)}) {
    _connection_map.remove(node);
    for (auto &&start : _connection_map.keys()) {
      _connection_map[start].removeAll(node);
    }
  }
  _blocks.remove(block);
}
