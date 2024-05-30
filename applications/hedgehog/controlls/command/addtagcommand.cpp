#include "addtagcommand.h"
#include "../../utility/tagutility.h"

#include <QDebug>

AddTagCommand::AddTagCommand(QStandardItemModel *tree_view_model,
                             QItemSelectionModel *selection_model, QString text)
    : _tree_view_model(tree_view_model), _selection_model(selection_model),
      _text(text) {
  QModelIndex parent_tag_index;
  if (_tree_view_model->rowCount() == 0) { // if there is no elements, add first
    parent_tag_index = _tree_view_model->invisibleRootItem()->index();
    _row_to_insert = 0;
  } else { // else add to row after selection
    QModelIndexList indexes = _selection_model->selectedIndexes();
    if (indexes.size() > 1)
      return;
    auto &&selected_index = indexes.at(0);
    parent_tag_index = selected_index.parent();
    if (parent_tag_index == _tree_view_model->invisibleRootItem()->index())
      return;
    _row_to_insert = selected_index.row() + 1;
  }

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
