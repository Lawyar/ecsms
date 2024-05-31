#include "addattributecommand.h"

AddAttributeCommand::AddAttributeCommand(
    QStandardItemModel *tree_view_model,
    QItemSelectionModel *tree_selection_model,
    QStandardItemModel *table_view_model,
    QItemSelectionModel *table_selection_model)
    : _tree_view_model(tree_view_model),
      _tree_selection_model(tree_selection_model),
      _table_view_model(table_view_model),
      _table_selection_model(table_selection_model) {}

void AddAttributeCommand::Execute() {
  if (!_tree_selection_model)
    return;
  QModelIndexList tree_indexes = _tree_selection_model->selectedIndexes();
  if (tree_indexes.size() == 1) {
    QStandardItem *parent_tag = _table_view_model->invisibleRootItem();

    int row_to_insert = _table_view_model->rowCount();
    parent_tag->insertRow(row_to_insert,
                          {new QStandardItem("attribute_name"),
                           new QStandardItem("attribute_value")});
    auto &&index_to_insert = _table_view_model->index(row_to_insert, 0);
    _table_selection_model->select(
        index_to_insert, QItemSelectionModel::ClearAndSelect);
    // ui->tableView->edit(index_to_insert);
  }
}

void AddAttributeCommand::UnExecute() {}
