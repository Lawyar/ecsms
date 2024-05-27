#include "selectionmodel.h"
#include "../events/repaintevent.h"

#include <QDebug>

const QMap<NodeId, std::vector<NodeId>> &
SelectionModel::GetSelectionMap() const {
  return _map_of_selected_nodes;
}

const std::set<BlockId> &SelectionModel::GetSelectedBlocks() const {
  return _selected_blocks;
}

void SelectionModel::AddSelection(const BlockId &block) {
  qDebug() << "add block to selection";
  _selected_blocks.insert(block);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::RemoveSelection(const BlockId &block) {
  qDebug() << "remove block from selection";
  _selected_blocks.erase(block);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::AddSelection(const NodeId &start,
                                  const NodeId &end) {
  _map_of_selected_nodes[start].push_back(end);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::RemoveSelection(const NodeId &start,
                                     const NodeId &end) {
  _map_of_selected_nodes[start].erase(
      std::find(_map_of_selected_nodes[start].begin(),
                _map_of_selected_nodes[start].end(), end));
  if (_map_of_selected_nodes[start].empty())
    _map_of_selected_nodes.remove(start);

  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::Clear() {
  _map_of_selected_nodes.clear();
  _selected_blocks.clear();
  Notify(std::make_shared<RepaintEvent>());
}
