#include "selectionmodel.h"
#include "../events/repaintevent.h"

const QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>> &
SelectionModel::GetSelectionMap() const {
  return _map_of_selected_nodes;
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
  Notify(std::make_shared<RepaintEvent>());
}
