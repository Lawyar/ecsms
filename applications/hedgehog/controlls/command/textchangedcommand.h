#pragma once

#include "icommand.h"

#include <QModelIndex>
#include <QStandardItemModel>
#include <QString>

class TagTextChangedCommand : public ICommand {
public:
  TagTextChangedCommand(QModelIndex index, QString old_text, QString new_text);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  QString _old_text, _new_text;
  int _row, _col;
  std::vector<int> _rows;
  const QStandardItemModel *_model;
};
