#include "addtagcommand.h"

AddTagCommand::AddTagCommand(QModelIndex parent_tag_index, QTreeView *tree_view,
                             QString text,
                             QStandardItemModel *attribute_table_view)
    : _parent_tag_index(parent_tag_index), _tree_view(tree_view), _text(text),
      _attribute_table_view(attribute_table_view) {}

static QStandardItem *createTag(QStandardItem *parent_tag,
                                QStandardItemModel *attribute_table_view,
                                const QString &text) {
  if (attribute_table_view == nullptr) {
    attribute_table_view = new QStandardItemModel();
    attribute_table_view->setHorizontalHeaderLabels(
        QStringList({"Attribute", "Value"}));
  }

  auto new_tag = new QStandardItem(text.isEmpty() ? "tag_name" : text);
  parent_tag->appendRow(new_tag);
  QVariant table_model_variant;
  table_model_variant.setValue(attribute_table_view);
  new_tag->setData(table_model_variant);
  new_tag->setFlags(new_tag->flags() | Qt::ItemIsEditable);
  return new_tag;
}

void AddTagCommand::Execute() {
  auto &&model = qobject_cast<QStandardItemModel *>(_tree_view->model());
  if (!model) {
    assert(false);
    return;
  }

  QStandardItem *parent_tag;
  if (_tree_view->model()->rowCount() == 0) {

    parent_tag = model->invisibleRootItem();
  } else {
    parent_tag = model->itemFromIndex(_parent_tag_index);
  }

  auto new_tag = createTag(parent_tag, nullptr, "");
  QVariant table_model_variant;
  table_model_variant.setValue(_attribute_table_view);
  new_tag->setData(table_model_variant);
  new_tag->setFlags(new_tag->flags() | Qt::ItemIsEditable);
  _tree_view->selectionModel()->select(new_tag->index(),
                                       QItemSelectionModel::ClearAndSelect);
  _tree_view->edit(new_tag->index());
}

void AddTagCommand::UnExecute() {
  if (_parent_tag_index.isValid())
    _tree_view->model()->removeRow(0, _parent_tag_index);
  else {
    _tree_view->model()->removeRow(0);
  }
}
