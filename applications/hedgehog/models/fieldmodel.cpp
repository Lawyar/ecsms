#include "fieldmodel.h"
#include "nodetype.h"
#include "../events/addblockevent.h"

const QMap<NodeId, std::vector<NodeId>> &FieldModel::GetConnectionMap() const {
  return _connection_map;
}

const QMap<BlockId, FieldModel::BlockData> &FieldModel::GetBlocks() const {
  return _blocks;
}

std::optional<FieldModel::BlockData>
FieldModel::GetBlockData(const BlockId &block) const {
  if (_blocks.contains(block))
    return _blocks[block];
  return std::nullopt;
}

void FieldModel::SetBlockData(const BlockId &block, BlockData bd) {
  _blocks[block].pos = bd.pos;
}

const QMap<NodeId, FieldModel::NodeData> &FieldModel::GetNodes() const {
  return _nodes;
}

std::optional<FieldModel::NodeData>
FieldModel::GetNodeData(const NodeId &node) const {
  if (_nodes.contains(node))
    return _nodes[node];
  return std::nullopt;
}

void FieldModel::Remove(const NodeId &start) { _connection_map.remove(start); }

void FieldModel::AddConnection(const NodeId &start, const NodeId &end) {
  _connection_map[start].push_back(end);
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
    if (std::find(current_heighbor_nodes.begin(), current_heighbor_nodes.end(),
                  node) != current_heighbor_nodes.end())
      return true;
  }
  return false;
}

void FieldModel::AddBlock(const BlockId &block, const BlockData &bd,
                          const QMap<NodeType, NodeData> &node_data_map) {
  _blocks[block] = bd;
  for (auto &&node_type : node_data_map.keys()) {
    auto &&id = block.GetChildId(static_cast<PartId>(node_type));
    _nodes[id] = node_data_map[node_type];
  }
  Notify(std::make_shared<AddBlockEvent>(block, bd.pos));
}

void FieldModel::RemoveBlock(const BlockId &block) {
  for (auto &&node :
       {block.GetChildId(static_cast<PartId>(NodeType::Incoming)),
        block.GetChildId(static_cast<PartId>(NodeType::Outgoing))}) {
    _connection_map.remove(node);
    for (auto &&start : _connection_map.keys()) {
      auto &&connections = _connection_map[start];
      auto &&iter = std::find(connections.begin(), connections.end(), node);
      _connection_map[start].erase(iter);
    }
  }
  _blocks.remove(block);
}
