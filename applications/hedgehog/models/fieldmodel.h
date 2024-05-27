#pragma once

#include "../connectnodewidget.h"
#include "../namemaker/blocknamemaker.h"
#include "imodel.h"

#include <QMap>
#include <optional>

class FieldModel : public IModel {
public:
  struct BlockData {
    QPoint pos;
    std::map<NodeType, QPoint> offset;
  };
  struct NodeData {
    NodeType node_type;
  };

public:
  FieldModel() = default;
  const QMap<NodeId, std::vector<NodeId>> &GetConnectionMap() const;
  const QMap<BlockId, BlockData> &GetBlocks() const;
  std::optional<BlockData> GetBlockData(const BlockId &block) const;
  void SetBlockData(const BlockId &block, BlockData bd);
  const QMap<NodeId, NodeData> &GetNodes() const;
  std::optional<NodeData> GetNodeData(const NodeId &node) const;
  void Remove(const NodeId &start);
  void AddConnection(const NodeId &start, const NodeId &end);
  void RemoveConnection(const NodeId &start, const NodeId &end);

  bool IsNodeUsed(const NodeId &node) const;
  void AddBlock(const BlockId &block, const BlockData &bd,
                const QMap<NodeType, NodeData> &node_data_map);
  void RemoveBlock(const BlockId &block);

private:
  QMap<NodeId, std::vector<NodeId>> _connection_map;
  QMap<BlockId, BlockData> _blocks;
  QMap<NodeId, NodeData> _nodes;
};
