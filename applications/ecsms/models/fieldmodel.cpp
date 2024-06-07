#include "fieldmodel.h"
#include "../events/addblockevent.h"
#include "../events/changeactivenodeevent.h"
#include "../events/removeblockevent.h"
#include "../events/repaintevent.h"
#include "../events/updateblockevent.h"
#include "../utility/containerutility.h"
#include "nodetype.h"

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
  if (_blocks[block].pos != bd.pos) {
    _blocks[block].pos = bd.pos;
    Notify(std ::make_shared<UpdateBlockEvent>(block, _blocks[block]));
  }
  _blocks[block].size = bd.size;
  _blocks[block].offset = bd.offset;
  _blocks[block].text = bd.text;
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

void FieldModel::AddConnection(const NodeId &start, const NodeId &end) {
  _connections[start].push_back(end);
  for (auto &&node : {start, end})
    Notify(std::make_shared<ChangeActiveNodeEvent>(node, true));
  Notify(std::make_shared<RepaintEvent>());
}

void FieldModel::RemoveConnection(const NodeId &start, const NodeId &end) {
  _connections[start].erase(
      std::find(_connections[start].begin(), _connections[start].end(), end));
  if (_connections[start].empty())
    _connections.remove(start);

  for (auto &&node : {start, end})
    Notify(
        std::make_shared<ChangeActiveNodeEvent>(node, IsNodeConnected(node)));

  Notify(std::make_shared<RepaintEvent>());
}

bool FieldModel::IsNodeConnected(const NodeId &node) const {
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
  QSet<NodeId> affected_nodes;

  for (auto &&node :
       {block.GetChildId(static_cast<PartId>(NodeType::Incoming)),
        block.GetChildId(static_cast<PartId>(NodeType::Outgoing))}) {
    _connections.remove(node); // delete node from "starts"
    affected_nodes.insert(node);
    for (auto &&start : _connections.keys()) { // delete node from "ends"
      affected_nodes.insert(start);
      auto &&connections = _connections[start];
      auto &&iter = std::find(connections.begin(), connections.end(), node);
      if (iter != connections.end())
        _connections[start].erase(iter);
    }
    _nodes.remove(node);
  }
  _blocks.remove(block);

  for (auto &&node : affected_nodes)
    Notify(
        std::make_shared<ChangeActiveNodeEvent>(node, IsNodeConnected(node)));

  Notify(std::make_shared<RemoveBlockEvent>(block));
}

void FieldModel::RemoveAll() { _blocks.clear(); }

FieldModel::Memento FieldModel::Save() const {
  Memento res{_connections, _blocks, _nodes};
  return res;
}

void FieldModel::Load(const Memento &m) {
  auto block_ids_to_create = SubstractionKeys(m._blocks, _blocks);
  auto block_ids_to_delete = SubstractionKeys(_blocks, m._blocks);
  auto &&blocks_to_update = IntersectionKeys(m._blocks, _blocks);

  _connections = m._connections;
  _blocks = m._blocks;
  _nodes = m._nodes;

  for (auto &&id : block_ids_to_create) {
    Notify(std::make_shared<AddBlockEvent>(id, _blocks[id]));
  }

  for (auto &&id : block_ids_to_delete) {
    Notify(std::make_shared<RemoveBlockEvent>(id));
  }

  for (auto &&node : _nodes.keys()) {
    Notify(
        std::make_shared<ChangeActiveNodeEvent>(node, IsNodeConnected(node)));
  }

  for (auto &&block : blocks_to_update) {
    Notify(std ::make_shared<UpdateBlockEvent>(block, _blocks[block]));
  }

  Notify(std::make_shared<RepaintEvent>());
}
