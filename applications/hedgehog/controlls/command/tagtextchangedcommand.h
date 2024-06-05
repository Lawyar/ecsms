#pragma once

#include "icommand.h"

#include <QModelIndex>
#include <QStandardItemModel>
#include <QString>
#include <QTreeView>

class TagTextChangedCommand : public ICommand {
public:
  TagTextChangedCommand(QModelIndex tag, QString old_text, QString new_text,
                        const QStandardItemModel *tree_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  QString _old_text, _new_text;
  std::vector<int> _rows;
  const QStandardItemModel *_tree_model;
};
