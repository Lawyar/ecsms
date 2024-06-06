#include "selectionmodel.h"
#include "../events/repaintevent.h"

#include <QDebug>

const QMap<NodeId, std::vector<NodeId>> &
SelectionModel::GetSelectedConnections() const {
  return _map_of_selected_nodes;
}

const std::set<BlockId> &SelectionModel::GetSelectedBlocks() const {
  return _selected_blocks;
}

void SelectionModel::AddSelection(const BlockId &block) {
  _selected_blocks.insert(block);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::RemoveSelection(const BlockId &block) {
  _selected_blocks.erase(block);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::RemoveSelectionWithNode(const NodeId &node) {
  _map_of_selected_nodes.remove(node);
  for (auto &&start : _map_of_selected_nodes.keys()) {
    for (auto &&end : _map_of_selected_nodes[start]) {
      auto &&connects = _map_of_selected_nodes[start];
      if (auto iter = std::find(connects.begin(), connects.end(), node);
          iter != connects.end())
        connects.erase(iter);
    }
  }
  _selected_blocks.erase(node.GetParentId());
}

void SelectionModel::AddSelection(const NodeId &start, const NodeId &end) {
  _map_of_selected_nodes[start].push_back(end);
  Notify(std::make_shared<RepaintEvent>());
}

void SelectionModel::RemoveSelection(const NodeId &start, const NodeId &end) {
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

SelectionModel::Memento SelectionModel::Save() const {
  Memento res{_map_of_selected_nodes, _selected_blocks};
  return res;
}

void SelectionModel::Load(const SelectionModel::Memento &m) {
  _map_of_selected_nodes = m._map_of_selected_nodes;
  _selected_blocks = m._selected_blocks;

  Notify(std::make_shared<RepaintEvent>());
}
