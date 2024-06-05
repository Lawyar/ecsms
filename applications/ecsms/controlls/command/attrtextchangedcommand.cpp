#include "attrtextchangedcommand.h"

AttrTextChangedCommand::AttrTextChangedCommand(QModelIndex tag_index, int row,
                                               int col, QString old_text,
                                               QString new_text,
                                               QStandardItemModel *tree_model)
    : _row(row), _col(col), _old_text(old_text), _new_text(new_text), _tree_model(tree_model) {
  _rows.insert(_rows.begin(), tag_index.row());
  auto &&parent_tag_index = tag_index.parent();
  while (parent_tag_index != _tree_model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent_tag_index.row());
    parent_tag_index = parent_tag_index.parent();
  }
}

void AttrTextChangedCommand::Execute() {
  QStandardItem *tag = _tree_model->invisibleRootItem();
  for (int row : _rows) {
    tag = tag->child(row, 0);
  }
  auto &&table_model =
      tag->data(Qt::UserRole + 1).value<QStandardItemModel *>();
  table_model->item(_row, _col)->setText(_new_text);
}

void AttrTextChangedCommand::UnExecute() {
  QStandardItem *tag = _tree_model->invisibleRootItem();
  for (int row : _rows) {
    tag = tag->child(row, 0);
  }
  auto &&table_model =
      tag->data(Qt::UserRole + 1).value<QStandardItemModel *>();
  table_model->item(_row, _col)->setText(_old_text);
}
