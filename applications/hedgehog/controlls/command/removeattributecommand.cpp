#include "removeattributecommand.h"

RemoveAttributeCommand::RemoveAttributeCommand(
    QModelIndex index_to_remove, QStandardItemModel *table_view_model)
    : _row_to_remove(index_to_remove.row()),
      _table_view_model(table_view_model) {}

void RemoveAttributeCommand::Execute() {
  _attribute_name = _table_view_model->item(_row_to_remove, 0)->text();
  _attribute_value = _table_view_model->item(_row_to_remove, 1)->text();
  _table_view_model->removeRow(_row_to_remove);
}

void RemoveAttributeCommand::UnExecute() {
  QStandardItem *parent_tag = _table_view_model->invisibleRootItem();
  parent_tag->insertRow(_row_to_remove, {new QStandardItem(_attribute_name),
                                         new QStandardItem(_attribute_value)});
}
