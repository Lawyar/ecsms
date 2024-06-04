#include "addchildtagcommand.h"
#include "../../utility/tagutility.h"

#include <QDebug>

AddChildTagCommand::AddChildTagCommand(QModelIndex parent_index,
                                       QStandardItemModel *tree_view_model)
    : _tree_view_model(tree_view_model) {
  if (parent_index == QModelIndex()) // to prevent adding child to invisible root item
    return;

  for (auto &&parent_tag_index = parent_index.parent();
       parent_tag_index != QModelIndex();
       parent_tag_index = parent_tag_index.parent()) {
    _rows.insert(_rows.begin(), parent_index.row());
    parent_index = parent_index.parent();
  }
}

void AddChildTagCommand::Execute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }

  auto new_tag = createTag(nullptr, "");
  parent_tag->insertRow(0, new_tag);
}

void AddChildTagCommand::UnExecute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  _tree_view_model->removeRow(0, parent_tag->index());
}
