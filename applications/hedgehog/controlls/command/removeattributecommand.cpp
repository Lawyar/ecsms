#include "removeattributecommand.h"

RemoveAttributeCommand::RemoveAttributeCommand(
    QModelIndex index_to_remove, QStandardItemModel *table_view_model,
    QItemSelectionModel *table_selection_model)
    : _row_to_remove(index_to_remove.row()),
      _table_view_model(table_view_model),
      _table_selection_model(table_selection_model) {}

void RemoveAttributeCommand::Execute() {
  _attribute_name = _table_view_model->item(_row_to_remove, 0)->text();
  _attribute_value = _table_view_model->item(_row_to_remove, 1)->text();
  _table_view_model->removeRow(_row_to_remove);

  // set selection to first item for tableView->selectionModel();
  auto &&index_to_select = _table_view_model->index(0, 0);
  _table_selection_model->setCurrentIndex(index_to_select,
                                          QItemSelectionModel::ClearAndSelect);
}

void RemoveAttributeCommand::UnExecute() {
  QStandardItem *parent_tag = _table_view_model->invisibleRootItem();
  parent_tag->insertRow(_row_to_remove, {new QStandardItem(_attribute_name),
                                         new QStandardItem(_attribute_value)});
  auto &&index_to_select = _table_view_model->index(_row_to_remove, 0);
  _table_selection_model->select(index_to_select,
                                 QItemSelectionModel::ClearAndSelect);
}
