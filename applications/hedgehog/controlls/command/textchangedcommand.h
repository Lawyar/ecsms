#pragma once

#include "icommand.h"

#include <QModelIndex>
#include <QStandardItemModel>
#include <QString>

class TextChangedCommand : public ICommand {
public:
  TextChangedCommand(QModelIndex tag, QString old_text, QString new_text);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  QString _old_text, _new_text;
  int _row, _col;
  std::vector<int> _rows;
  const QStandardItemModel *_tree_model;
};
