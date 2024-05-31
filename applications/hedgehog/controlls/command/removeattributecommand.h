#pragma once

#include "icommand.h"

#include <QStandardItemModel>

class RemoveAttributeCommand : public ICommand {
public:
  RemoveAttributeCommand(QModelIndex index_to_remove,
                         QStandardItemModel *table_view_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  int _row_to_remove;
  QStandardItemModel *_table_view_model;
  QString _attribute_name, _attribute_value;
};
