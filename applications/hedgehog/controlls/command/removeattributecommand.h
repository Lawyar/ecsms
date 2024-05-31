#pragma once

#include "icommand.h"

#include <QItemSelectionModel>
#include <QStandardItemModel>

class RemoveAttributeCommand : public ICommand {
public:
  RemoveAttributeCommand(QModelIndex index_to_remove,
                         QStandardItemModel *table_view_model,
                         QItemSelectionModel *table_selection_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  int _row_to_remove;
  QStandardItemModel *_table_view_model;
  QItemSelectionModel *_table_selection_model;
  QString _attribute_name, _attribute_value;
};
