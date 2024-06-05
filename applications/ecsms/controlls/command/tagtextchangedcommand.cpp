#include "tagtextchangedcommand.h"

#include <QDebug>
#include <QStandardItemModel>

TagTextChangedCommand::TagTextChangedCommand(QModelIndex tag, QString old_text,
                                             QString new_text,
                                             const QStandardItemModel *tree_model)
    : _old_text(old_text), _new_text(new_text), _tree_model(tree_model) {
  _rows.insert(_rows.begin(), tag.row());
  auto &&parent_tag_index = tag.parent();
  while (parent_tag_index != _tree_model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent_tag_index.row());
    parent_tag_index = parent_tag_index.parent();
  }
}

void TagTextChangedCommand::Execute() {
  QStandardItem *tag = _tree_model->invisibleRootItem();
  for (int row : _rows) {
    tag = tag->child(row, 0);
  }
  tag->setText(_new_text);
}

void TagTextChangedCommand::UnExecute() {
  QStandardItem *tag = _tree_model->invisibleRootItem();
  for (int row : _rows) {
    tag = tag->child(row, 0);
  }
  tag->setText(_old_text);
}
