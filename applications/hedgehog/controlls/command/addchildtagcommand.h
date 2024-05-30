#pragma once

#include "icommand.h"

#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QTreeView>

class AddChildTagCommand : public ICommand {
public:
  AddChildTagCommand(QTreeView *tree_view, QString text);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  std::vector<int> _rows;
  int _row_to_insert;
  QTreeView *_tree_view;
  QString _text;
};
