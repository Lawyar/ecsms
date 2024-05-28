#include "fieldmodel.h"
#include "../events/addblockevent.h"
#include "../events/removeblockevent.h"
#include "../events/repaintevent.h"
#include "nodetype.h"
#include "../utility/containerutility.h"

const QMap<NodeId, std::vector<NodeId>> &FieldModel::GetConnectionMap() const {
  return _connections;
}

QMap<NodeId, std::vector<NodeId>>
FieldModel::GetNodeConnections(NodeId node) const {
  QMap<NodeId, std::vector<NodeId>> res;
  res[node] = _connections[node];            // find if node is start
  for (auto &&start : _connections.keys()) { // find if node is end
    auto &&connects = _connections[start];
    auto &&connect = std::find(connects.begin(), connects.end(), node);
    if (connect != connects.end())
      res[start].push_back(*connect);
  }
  return res;
}

QMap<NodeId, QMap<NodeId, std::vector<NodeId>>>
FieldModel::GetBlockConnections(BlockId block) const {
  QMap<NodeId, QMap<NodeId, std::vector<NodeId>>> res;
  auto &&l_n_id = block.GetChildId(static_cast<PartId>(NodeType::Incoming));
  auto &&r_n_id = block.GetChildId(static_cast<PartId>(NodeType::Outgoing));
  res[l_n_id] = GetNodeConnections(l_n_id);
  res[r_n_id] = GetNodeConnections(r_n_id);
  return res;
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

void FieldModel::Remove(const NodeId &start) { _connections.remove(start); }

void FieldModel::AddConnection(const NodeId &start, const NodeId &end) {
  _connections[start].push_back(end);
  Notify(std::make_shared<RepaintEvent>());
}

void FieldModel::RemoveConnection(const NodeId &start, const NodeId &end) {
  _connections[start].erase(
      std::find(_connections[start].begin(), _connections[start].end(), end));
  if (_connections[start].empty())
    _connections.remove(start);
}

bool FieldModel::IsNodeUsed(const NodeId &node) const {
  for (auto &&start : _connections.keys()) {
    auto &&connections = _connections[start];
    if (node == start)
      return true;
    if (std::find(connections.begin(), connections.end(), node) !=
        connections.end())
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
  Notify(std::make_shared<AddBlockEvent>(block, bd));
}

void FieldModel::RemoveBlock(const BlockId &block) {
  for (auto &&node :
       {block.GetChildId(static_cast<PartId>(NodeType::Incoming)),
        block.GetChildId(static_cast<PartId>(NodeType::Outgoing))}) {
    _connections.remove(node);                 // delete node from "starts"
    for (auto &&start : _connections.keys()) { // delete node from "ends"
      auto &&connections = _connections[start];
      auto &&iter = std::find(connections.begin(), connections.end(), node);
      if (iter != connections.end())
        _connections[start].erase(iter);
    }
  }
  _blocks.remove(block);
  Notify(std::make_shared<RemoveBlockEvent>(block));
}

FieldModel::Memento FieldModel::Save() const {
  Memento res{_connections, _blocks, _nodes};
  return res;
}

void FieldModel::Load(const Memento &m) {
  auto block_ids_to_create = SubstractionKeys(m._blocks, _blocks);
  auto block_ids_to_delete = SubstractionKeys(_blocks, m._blocks);

  _connections = m._connections;
  _blocks = m._blocks;
  _nodes = m._nodes;

  for (auto &&id : block_ids_to_create) {
    Notify(std::make_shared<AddBlockEvent>(id, _blocks[id]));
  }

  for (auto &&id : block_ids_to_delete) {
    Notify(std::make_shared<RemoveBlockEvent>(id));
  }
}
