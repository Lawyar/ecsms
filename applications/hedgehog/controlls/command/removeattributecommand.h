#pragma once

#include "icommand.h"

#include <QStandardItemModel>

class RemoveAttributeCommand : public ICommand {
public:
  RemoveAttributeCommand(int table_row_to_remove,
                         QModelIndex tag_index,
                         QStandardItemModel *tree_view_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  int _table_row_to_remove;
  QString _attribute_name, _attribute_value;
  std::vector<int> _tree_rows;
  QStandardItemModel *_tree_view_model;
};
