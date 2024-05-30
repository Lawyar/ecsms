#pragma once

#include <QStandardItem>
#include <QStandardItemModel>

inline QStandardItemModel *createEmptyTableModel() {
  QStandardItemModel *attribute_table_view = new QStandardItemModel();
  attribute_table_view->setHorizontalHeaderLabels(
      QStringList({"Attribute", "Value"}));
  return attribute_table_view;
}

inline QStandardItem *createTag(QStandardItemModel *attributes, const QString &text) {
  auto new_tag = new QStandardItem(text.isEmpty() ? "tag_name" : text);
  QVariant table_model_variant;
  if (!attributes)
    attributes = createEmptyTableModel();
  table_model_variant.setValue(attributes);
  new_tag->setData(table_model_variant);
  new_tag->setFlags(new_tag->flags() | Qt::ItemIsEditable);
  return new_tag;
}

inline QStandardItem *copyTag(QStandardItem *tag) {
  QStandardItem *res = tag->clone();

  auto &&table_model = createEmptyTableModel();
  auto &&attributes = tag->data(Qt::UserRole + 1).value<QStandardItemModel *>();
  for (int i = 0; i < attributes->rowCount(); ++i) {
    table_model->appendRow(
        {attributes->item(i, 0)->clone(), attributes->item(i, 1)->clone()});
  }

  for (int i = 0; i < tag->rowCount(); ++i) {
    /*
    auto &&table_model = createEmptyTableModel();
    table_model = createEmptyTableModel();
    auto &&attributes = tag->child(i, 0)
                            ->data(Qt::UserRole + 1)
                            .value<QStandardItemModel *>();
    for (int i = 0; i < attributes->rowCount(); ++i) {
      table_model->appendRow(
          {attributes->item(i, 0)->clone(), attributes->item(i, 1)->clone()});
    }

    auto new_tag = createTag(table_model, tag->child(i, 0)->text());
    
    */
    res->appendRow(copyTag(tag->child(i, 0)));
  }

  res->setText(tag->text());

  return res;
}
