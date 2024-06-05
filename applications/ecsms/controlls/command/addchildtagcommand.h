#pragma once

#include "icommand.h"

#include <QModelIndex>
#include <QStandardItemModel>
#include <QTreeView>

class AddChildTagCommand : public ICommand {
public:
  AddChildTagCommand(QModelIndex parent_index,
                     QStandardItemModel *tree_view_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  std::vector<int> _rows;
  QStandardItemModel *_tree_view_model;
};
