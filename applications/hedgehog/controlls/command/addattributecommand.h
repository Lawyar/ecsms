#pragma once

#include "icommand.h"

#include <QItemSelectionModel>
#include <QStandardItemModel>

class AddAttributeCommand : public ICommand {
public:
  AddAttributeCommand(int row_to_insert,
                      QStandardItemModel *table_view_model,
                      QItemSelectionModel *table_selection_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  int _row_to_insert;
  QStandardItemModel *_table_view_model;
  QItemSelectionModel *_table_selection_model;
};
