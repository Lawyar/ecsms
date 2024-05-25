#include "selectionmodel.h"
const QMap<const ConnectNodeWidget *, QVector<const ConnectNodeWidget *>> &
SelectionModel::GetSelectionMap() const {
  return _map_of_selected_nodes;
}

void SelectionModel::AddSelection(const ConnectNodeWidget *start,
                                  const ConnectNodeWidget *end) {
  _map_of_selected_nodes[start].append(end);
}

void SelectionModel::RemoveSelection(const ConnectNodeWidget *start,
                                      const ConnectNodeWidget *end) {
  _map_of_selected_nodes[start].erase(
      std::find(_map_of_selected_nodes[start].begin(),
                _map_of_selected_nodes[start].end(), end));
}

void SelectionModel::Clear() { _map_of_selected_nodes.clear(); }
