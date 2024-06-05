#include "addattributecommand.h"

AddAttributeCommand::AddAttributeCommand(int table_row_to_insert,
                                         QModelIndex tag_index,
                                         QStandardItemModel *tree_view_model)
    : _table_row_to_insert(table_row_to_insert),
      _tree_view_model(tree_view_model) {
  _tree_rows.insert(_tree_rows.begin(), tag_index.row());

  for (auto &&parent_index = tag_index.parent();
       parent_index != _tree_view_model->invisibleRootItem()->index();
       parent_index = parent_index.parent()) {
    _tree_rows.insert(_tree_rows.begin(), parent_index.row());
  }
}

void AddAttributeCommand::Execute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _tree_rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  auto parent_attributes =
      parent_tag->data(Qt::UserRole + 1).value<QStandardItemModel *>();
  parent_attributes->insertRow(_table_row_to_insert,
                               {new QStandardItem("attribute_name"),
                                new QStandardItem("attribute_value")});
}

void AddAttributeCommand::UnExecute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _tree_rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  auto parent_attributes =
      parent_tag->data(Qt::UserRole + 1).value<QStandardItemModel *>();
  parent_attributes->removeRow(_table_row_to_insert);
}
