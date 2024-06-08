#pragma once

#include "../widgets/blockwidget.h"
#include "../widgets/connectnodewidget.h"
#include "imodel.h"

#include <QMap>
#include <set>

class SelectionModel : public IModel {
public:
  SelectionModel() = default;

  SelectionModel &operator=(const SelectionModel &other);

  const QMap<NodeId, std::vector<NodeId>> &GetSelectedConnections() const;
  const std::set<BlockId> &GetSelectedBlocks() const;

  void AddSelection(const BlockId &block);
  void RemoveSelection(const BlockId &block);
  void RemoveSelectionWithNode(const NodeId &node);
  void AddSelection(const NodeId &start, const NodeId &end);
  void RemoveSelection(const NodeId &start, const NodeId &end);

  void Clear();
  class Memento;
  Memento Save() const;
  void Load(const Memento &m);

  class Memento {
  public:
    Memento(const Memento &) = delete;
    Memento(Memento &&) = default;
    Memento &operator=(const Memento &) = delete;
    Memento &operator=(Memento &&) = delete;

  private:
    Memento(QMap<NodeId, std::vector<NodeId>> map_of_selected_nodes,
            std::set<BlockId> selected_blocks)
        : _map_of_selected_nodes(map_of_selected_nodes),
          _selected_blocks(selected_blocks) {}
    QMap<NodeId, std::vector<NodeId>> _map_of_selected_nodes;
    std::set<BlockId> _selected_blocks;
    friend SelectionModel;
  };

private:
  QMap<NodeId, std::vector<NodeId>> _map_of_selected_nodes;
  std::set<BlockId> _selected_blocks;
};
