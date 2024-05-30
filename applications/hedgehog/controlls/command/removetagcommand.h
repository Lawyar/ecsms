#pragma once

#include "icommand.h"

#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QTreeView>
#include <QTableView>

class RemoveTagCommand : public ICommand {
public:
  RemoveTagCommand(QTreeView *tree_view, QTableView *table_view);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  std::vector<int> _rows;
  int _row_to_remove;
  QString _text;
  QTreeView *_tree_view;
  QTableView *_table_view;
  QStandardItem *_item;
};
