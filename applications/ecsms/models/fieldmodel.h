#pragma once

#include "../namemaker/id.h"
#include "../namemaker/namemaker.h"
#include "../widgets/connectnodewidget.h"
#include "imodel.h"

#include <QMap>
#include <optional>

class FieldModel : public IModel {
public:
  struct BlockData {
    QPoint pos;
    std::map<NodeType, QPoint> offset;
    QString text;
  };
  struct NodeData {
    NodeType node_type;
  };

  class Memento {
  public:
    Memento(const Memento &) = delete;
    Memento(Memento &&) = default;
    Memento &operator=(const Memento &) = delete;
    Memento &operator=(Memento &&) = delete;

  private:
    Memento(QMap<NodeId, std::vector<NodeId>> connections,
            QMap<BlockId, BlockData> blocks, QMap<NodeId, NodeData> nodes)
        : _connections(connections), _blocks(blocks), _nodes(nodes) {}
    QMap<NodeId, std::vector<NodeId>> _connections;
    QMap<BlockId, BlockData> _blocks;
    QMap<NodeId, NodeData> _nodes;
    friend FieldModel;
  };

public:
  FieldModel() = default;

  const QMap<NodeId, std::vector<NodeId>> &GetConnectionMap() const;
  QMap<NodeId, std::vector<NodeId>> GetNodeConnections(NodeId node) const;

  QMap<NodeId, QMap<NodeId, std::vector<NodeId>>>
  GetBlockConnections(BlockId block) const;
  const QMap<BlockId, BlockData> &GetBlocks() const;
  std::optional<BlockData> GetBlockData(const BlockId &block) const;
  void SetBlockData(const BlockId &block, BlockData bd);

  const QMap<NodeId, NodeData> &GetNodes() const;
  std::optional<NodeData> GetNodeData(const NodeId &node) const;

  void AddConnection(const NodeId &start, const NodeId &end);
  void RemoveConnection(const NodeId &start, const NodeId &end);
  bool IsNodeConnected(const NodeId &node) const;
  void AddBlock(const BlockId &block, const BlockData &bd,
                const QMap<NodeType, NodeData> &node_data_map);
  void RemoveBlock(const BlockId &block);
  void RemoveAll();

  Memento Save() const;
  void Load(const Memento &m);

private:
  QMap<NodeId, std::vector<NodeId>> _connections;
  QMap<BlockId, BlockData> _blocks;
  QMap<NodeId, NodeData> _nodes;
};
