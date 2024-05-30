#pragma once

#include <QStandardItem>
#include <QStandardItemModel>

inline QStandardItemModel *createEmptyTableModel(QStandardItem *tag) {
  QStandardItemModel *attribute_table_view = new QStandardItemModel();
  attribute_table_view->setHorizontalHeaderLabels(
      QStringList({"Attribute", "Value"}));
  return attribute_table_view;
}

inline QStandardItem *createEmptyTag(QStandardItem *parent_tag,
                                     const QString &text) {
  auto new_tag = new QStandardItem(text.isEmpty() ? "tag_name" : text);
  QVariant table_model_variant;
  table_model_variant.setValue(createEmptyTableModel(new_tag));
  new_tag->setData(table_model_variant);
  new_tag->setFlags(new_tag->flags() | Qt::ItemIsEditable);
  return new_tag;
}
