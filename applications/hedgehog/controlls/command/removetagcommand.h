#pragma once

#include "icommand.h"

#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStandardItemModel>

class RemoveTagCommand : public ICommand {
public:
  RemoveTagCommand(QModelIndex index, QStandardItemModel *tree_view_model,
                   QItemSelectionModel *selection_model, QString text);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  std::vector<int> _rows;
  QStandardItemModel *_tree_view_model;
  QItemSelectionModel *_selection_model;
  QString _text;
  QStandardItemModel *_table_model;
};
