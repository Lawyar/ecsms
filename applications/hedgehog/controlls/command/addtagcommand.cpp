#include "addtagcommand.h"
#include "../../utility/tagutility.h"

#include <QDebug>

AddTagCommand::AddTagCommand(QModelIndex index_before_insert,
                             QStandardItemModel *tree_view_model)
    : _row_to_insert(index_before_insert.row() + 1),
      _tree_view_model(tree_view_model) {
  for (auto &&parent_tag_index = index_before_insert.parent();
       parent_tag_index != QModelIndex();
       parent_tag_index = parent_tag_index.parent()) {
    _rows.insert(_rows.begin(), parent_tag_index.row());
  }
}

void AddTagCommand::Execute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  auto new_tag = createTag(nullptr, "");
  parent_tag->insertRow(_row_to_insert, new_tag);
}

void AddTagCommand::UnExecute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  _tree_view_model->removeRow(_row_to_insert, parent_tag->index());
}
