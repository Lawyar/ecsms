#include "addchildtagcommand.h"
#include "../../utility/tagutility.h"

#include <QDebug>

AddChildTagCommand::AddChildTagCommand(QTreeView *tree_view, QString text)
    : _tree_view(tree_view), _text(text) {
  QModelIndexList indexes = _tree_view->selectionModel()->selectedIndexes();
  if (indexes.size() > 1)
    return;
  QModelIndex parent_tag_index = indexes.at(0);
  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(_tree_view->model());
  if (!tree_view_model) {
    assert(false);
    return;
  }
  if (parent_tag_index == tree_view_model->invisibleRootItem()->index())
    return;
  _row_to_insert = 0;

  while (parent_tag_index != tree_view_model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent_tag_index.row());
    parent_tag_index = parent_tag_index.parent();
  }
}

void AddChildTagCommand::Execute() {
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

  auto new_tag = createTag(nullptr, _text);
  parent_tag->insertRow(_row_to_insert, new_tag);

  if (auto &&selected_tag =
          _tree_view_model->itemFromIndex(parent_tag->index())) {
    auto &&tag_text = selected_tag->text();
    auto &&tag_text_vec = tag_text.split(": ");
    if (tag_text_vec.size() > 1) {
      selected_tag->setText(tag_text_vec[0]);
    }
  }

  _tree_view->selectionModel()->select(new_tag->index(),
                           QItemSelectionModel::ClearAndSelect);
  _tree_view->expand(parent_tag->index());
}

void AddChildTagCommand::UnExecute() {
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
  _tree_view_model->removeRow(_row_to_insert, parent_tag->index());
  _tree_view->selectionModel()->select(parent_tag->index(),
                           QItemSelectionModel::ClearAndSelect);
}
