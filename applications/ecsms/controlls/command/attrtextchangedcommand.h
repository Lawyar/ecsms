#pragma once

#include "icommand.h"

#include <QModelIndex>
#include <QStandardItemModel>

class AttrTextChangedCommand : public ICommand {
public:
  AttrTextChangedCommand(QModelIndex tag_index, int row, int col,
                         QString old_text, QString new_text,
                         QStandardItemModel *tree_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  QString _old_text, _new_text;
  int _row, _col;
  std::vector<int> _rows;
  QStandardItemModel *_tree_model;
};
