#include "addtagcommand.h"
#include <QDebug>

AddTagCommand::AddTagCommand(QModelIndex parent_tag_index, int row_to_insert,
                             QStandardItemModel *tree_view_model,
                             QItemSelectionModel *selection_model, QString text)
    : _row_to_insert(row_to_insert), _tree_view_model(tree_view_model),
      _selection_model(selection_model), _text(text) {
  auto &&parent = parent_tag_index;
  while (parent != _tree_view_model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent.row());
    parent = parent_tag_index.parent();
  }
}

static QStandardItemModel *createEmptyTableModel(QStandardItem *tag) {
  QStandardItemModel *attribute_table_view = new QStandardItemModel();
  attribute_table_view->setHorizontalHeaderLabels(
      QStringList({"Attribute", "Value"}));
  return attribute_table_view;
}

static QStandardItem *createEmptyTag(QStandardItem *parent_tag,
                                     const QString &text) {
  auto new_tag = new QStandardItem(text.isEmpty() ? "tag_name" : text);
  QVariant table_model_variant;
  table_model_variant.setValue(createEmptyTableModel(new_tag));
  new_tag->setData(table_model_variant);
  new_tag->setFlags(new_tag->flags() | Qt::ItemIsEditable);
  return new_tag;
}

void AddTagCommand::Execute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }

  auto new_tag = createEmptyTag(parent_tag, "");
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

void AddTagCommand::UnExecute() {
  QStandardItem *parent_tag = _tree_view_model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, 0);
  }
  _tree_view_model->removeRow(_row_to_insert, parent_tag->index());
  _selection_model->select(parent_tag->index(),
                                         QItemSelectionModel::ClearAndSelect);
}
