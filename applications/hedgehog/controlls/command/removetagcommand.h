#pragma once

#include "icommand.h"

#include <QModelIndex>
#include <QStandardItemModel>

class RemoveTagCommand : public ICommand {
public:
  RemoveTagCommand(QModelIndex index_to_remove,
                   QStandardItemModel *tree_view_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  std::vector<int> _rows;
  int _row_to_remove;
  QString _text;
  QStandardItemModel *_tree_view_model, *_table_view_model;
  QStandardItem *_item;
};
