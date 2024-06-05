#pragma once

#include "icommand.h"

#include <QStandardItemModel>

class AddAttributeCommand : public ICommand {
public:
  AddAttributeCommand(int table_row_to_insert, QModelIndex tag_index,
                      QStandardItemModel *tree_view_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  int _table_row_to_insert;
  std::vector<int> _tree_rows;
  QStandardItemModel *_tree_view_model;
};
