#include "removetagcommand.h"
#include "../../utility/tagutility.h"

RemoveTagCommand::RemoveTagCommand(QTreeView *tree_view, QTableView *table_view)
    : _tree_view(tree_view), _table_view(table_view) {
  auto &&_selection_model = _tree_view->selectionModel();
  QModelIndexList indexes = _selection_model->selectedIndexes();
  if (indexes.size() > 1)
    return;
  auto &&selected_index = indexes.at(0);
  auto &&parent_tag_index = selected_index.parent();
  _row_to_remove = selected_index.row();

  auto &&_tree_view_model =
      qobject_cast<QStandardItemModel *>(_tree_view->model());
  if (!_tree_view_model) {
    assert(false);
    return;
  }

  while (parent_tag_index != _tree_view_model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent_tag_index.row());
    parent_tag_index = parent_tag_index.parent();
  }
}

void RemoveTagCommand::Execute() {
  auto &&_tree_view_model =
      qobject_cast<QStandardItemModel *>(_tree_view->model());
  if (!_tree_view_model) {
    assert(false);
    return;
  }

  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  auto &&tag_to_remove = parent_tag->child(_row_to_remove, 0);
  _item = copyTag(tag_to_remove);
  _tree_view_model->removeRow(_row_to_remove, parent_tag->index());
  _table_view->setModel(nullptr);
}

void RemoveTagCommand::UnExecute() {
  auto &&_tree_view_model =
      qobject_cast<QStandardItemModel *>(_tree_view->model());
  if (!_tree_view_model) {
    assert(false);
    return;
  }

  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }

  // auto new_tag = createTag(_table_model, _text);
  parent_tag->insertRow(_row_to_remove, _item);
  _tree_view->expand(_item->index());
}
