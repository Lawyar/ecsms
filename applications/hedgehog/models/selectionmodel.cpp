#include "selectionmodel.h"
#include "../events/repaintevent.h"

#include <QDebug>

const QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>> &
SelectionModel::GetSelectionMap() const {
  return _map_of_selected_nodes;
}

const QSet<BlockWidget *> &SelectionModel::GetSelectedBlocks() const {
  return _selected_blocks;
}

void SelectionModel::AddSelection(BlockWidget *block) {
  qDebug() << "add block to selection";
  _selected_blocks.insert(block);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::RemoveSelection(BlockWidget *block) {
  qDebug() << "remove block from selection";
  _selected_blocks.remove(block);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::AddSelection(ConnectNodeWidget *start,
                                  ConnectNodeWidget *end) {
  _map_of_selected_nodes[start].append(end);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::RemoveSelection(ConnectNodeWidget *start,
                                     ConnectNodeWidget *end) {
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
