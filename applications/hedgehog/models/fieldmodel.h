#pragma once

#include "../connectnodewidget.h"
#include "../namemaker/blocknamemaker.h"
#include "imodel.h"

#include <QMap>

class FieldModel : public IModel {
public:
  struct BlockData {
    QPoint pos;
  };

public:
  FieldModel() = default;
  const QMap<NodeId, QVector<NodeId>> &GetConnectionMap() const;
  void Remove(const NodeId &start);
  void AddConnection(const NodeId &start, const NodeId &end);
  void RemoveConnection(const NodeId &start, const NodeId &end);

  bool IsNodeUsed(const NodeId &node) const;
  void AddBlock(const BlockId &block, const BlockData &bd);
  void RemoveBlock(const BlockId &block);

private:
  QMap<NodeId, QVector<NodeId>> _connection_map;
  QMap<BlockId, BlockData> _blocks;
};
