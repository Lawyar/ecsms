#pragma once

#include "icommand.h"

#include <QModelIndex>
#include <QStandardItemModel>
#include <QItemSelectionModel>

class AddTagCommand : public ICommand {
public:
  AddTagCommand(QModelIndex parent_tag_index, int row_to_insert,
                QStandardItemModel *tree_view_model,
                QItemSelectionModel *selection_model, QString text);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  std::vector<int> _rows;
  int _row_to_insert;
  QStandardItemModel *_tree_view_model;
  QItemSelectionModel *_selection_model;
  QString _text;
};
