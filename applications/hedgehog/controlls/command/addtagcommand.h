#pragma once

#include "icommand.h"

#include <QModelIndex>
#include <QStandardItemModel>
#include <QTreeView>

class AddTagCommand : public ICommand {
public:
  AddTagCommand(QModelIndex parent_tag_index,
                QTreeView *tree_view, QString text,
                QStandardItemModel *attribute_table_view);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  QModelIndex _parent_tag_index;
  QTreeView *_tree_view;
  QString _text;
  QStandardItemModel *_attribute_table_view;
};
