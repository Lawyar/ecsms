#pragma once

#include "../blockwidget.h"
#include "../connectnodewidget.h"
#include "imodel.h"

#include <QMap>

class SelectionModel : public IModel {
public:
  SelectionModel() = default;
  const QMap<NodeId, QVector<NodeId>> &GetSelectionMap() const;
  const QSet<BlockId> &GetSelectedBlocks() const;
  void AddSelection(const BlockId &block);
  void RemoveSelection(const BlockId &block);
  void AddSelection(const NodeId &start, const NodeId &end);
  void RemoveSelection(const NodeId &start, const NodeId &end);
  void Clear();

private:
  QMap<NodeId, QVector<NodeId>> _map_of_selected_nodes;
  QSet<BlockId> _selected_blocks;
};
