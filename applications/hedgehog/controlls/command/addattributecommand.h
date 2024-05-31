#pragma once

#include "icommand.h"

#include <QItemSelectionModel>
#include <QStandardItemModel>

class AddAttributeCommand : public ICommand {
public:
  AddAttributeCommand(QStandardItemModel *tree_view_model,
                      QItemSelectionModel *tree_selection_model,
                      QStandardItemModel *table_view_model,
                      QItemSelectionModel *table_selection_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  QStandardItemModel *_tree_view_model, *_table_view_model;
  QItemSelectionModel *_tree_selection_model, *_table_selection_model;
};
