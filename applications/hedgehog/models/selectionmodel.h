#pragma once

#include "../blockwidget.h"
#include "../connectnodewidget.h"
#include "imodel.h"

#include <QMap>
#include <set>

class SelectionModel : public IModel {
public:
  SelectionModel() = default;
  const QMap<NodeId, std::vector<NodeId>> &GetSelectionMap() const;
  const std::set<BlockId> &GetSelectedBlocks() const;
  void AddSelection(const BlockId &block);
  void RemoveSelection(const BlockId &block);
  void RemoveSelectionWithNode(const NodeId &node);
  void AddSelection(const NodeId &start, const NodeId &end);
  void RemoveSelection(const NodeId &start, const NodeId &end);
  void Clear();

private:
  QMap<NodeId, std::vector<NodeId>> _map_of_selected_nodes;
  std::set<BlockId> _selected_blocks;
};
