#include "removetagcommand.h"
#include "../../utility/tagutility.h"

RemoveTagCommand::RemoveTagCommand(QModelIndex index_to_remove,
                                   QStandardItemModel *tree_view_model)
    : _tree_view_model(tree_view_model) {
  auto &&parent_tag_index = index_to_remove.parent();
  _row_to_remove = index_to_remove.row();

  while (parent_tag_index != _tree_view_model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent_tag_index.row());
    parent_tag_index = parent_tag_index.parent();
  }
}

void RemoveTagCommand::Execute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  auto &&tag_to_remove = parent_tag->child(_row_to_remove, 0);
  _item = copyTag(tag_to_remove);
  _tree_view_model->removeRow(_row_to_remove, parent_tag->index());
}

void RemoveTagCommand::UnExecute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }

  parent_tag->insertRow(_row_to_remove, _item);
}
