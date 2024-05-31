#include "addtagcommand.h"
#include "../../utility/tagutility.h"

#include <QDebug>

AddTagCommand::AddTagCommand(QModelIndex index_before_insert,
                             QStandardItemModel *tree_view_model,
                             QItemSelectionModel *selection_model, QString text)
    : _row_to_insert(index_before_insert.row() + 1), _tree_view_model(tree_view_model),
      _selection_model(selection_model), _text(text) {
  auto &&parent_tag_index = index_before_insert.parent();
  while (parent_tag_index != _tree_view_model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent_tag_index.row());
    parent_tag_index = parent_tag_index.parent();
  }
}

void AddTagCommand::Execute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }

  auto new_tag = createTag(nullptr, _text);
  parent_tag->insertRow(_row_to_insert, new_tag);

  _selection_model->select(new_tag->index(),
                           QItemSelectionModel::ClearAndSelect);
}

void AddTagCommand::UnExecute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  _tree_view_model->removeRow(_row_to_insert, parent_tag->index());
  _selection_model->select(parent_tag->index(),
                           QItemSelectionModel::ClearAndSelect);
}
