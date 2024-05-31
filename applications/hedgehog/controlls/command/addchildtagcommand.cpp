#include "addchildtagcommand.h"
#include "../../utility/tagutility.h"

#include <QDebug>

AddChildTagCommand::AddChildTagCommand(QModelIndex parent_index,
                                       QStandardItemModel *tree_view_model,
                                       QItemSelectionModel *selection_model,
                                       QString text)
    : _tree_view_model(tree_view_model), _selection_model(selection_model),
      _text(text) {
  if (parent_index == _tree_view_model->invisibleRootItem()->index())
    return;
  _row_to_insert = 0;

  while (parent_index != _tree_view_model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent_index.row());
    parent_index = parent_index.parent();
  }
}

void AddChildTagCommand::Execute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }

  auto new_tag = createTag(nullptr, _text);
  parent_tag->insertRow(_row_to_insert, new_tag);

  if (auto &&selected_tag =
          _tree_view_model->itemFromIndex(parent_tag->index())) {
    auto &&tag_text = selected_tag->text();
    auto &&tag_text_vec = tag_text.split(": ");
    if (tag_text_vec.size() > 1) {
      selected_tag->setText(tag_text_vec[0]);
    }
  }

  _selection_model->select(new_tag->index(),
                           QItemSelectionModel::ClearAndSelect);
}

void AddChildTagCommand::UnExecute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  _tree_view_model->removeRow(_row_to_insert, parent_tag->index());
  _selection_model->select(parent_tag->index(),
                           QItemSelectionModel::ClearAndSelect);
}
