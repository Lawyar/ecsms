#pragma once

#include "tagutility.h"

#include <QDebug>
#include <QFileDialog>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QStack>
#include <QStandardItem>
#include <QXmlStreamWriter>

template <class SaveFunc, class DontSaveFunc, class CancelFunc>
void openSavingMessageBox(SaveFunc save_func, DontSaveFunc dont_save_func,
                          CancelFunc cancel_func) {
  QMessageBox msg_box;
  msg_box.setWindowTitle("Внимание");
  msg_box.setText("Сохранить изменения?");
  msg_box.addButton("Сохранить", QMessageBox::YesRole);
  msg_box.addButton("Не сохранять", QMessageBox::NoRole);
  msg_box.addButton("Отменить", QMessageBox::RejectRole);
  msg_box.setIcon(QMessageBox::Warning);
  switch (msg_box.exec()) {
  case 0: {
    save_func();
    break;
  }
  case 1: {
    dont_save_func();
    break;
  }
  case 2: {
    cancel_func();
    break;
  }
  default: {
    assert(false);
    return;
  }
  }
}

inline void connectProcessOutputWithWidget(QProcess *process,
                                           QTextEdit *output) {
  auto start = [](QProcess *process) { process->start("cmd.exe", {"/U"}); };
  start(process);

  QObject::connect(process, &QProcess::readyRead, [process, output]() {
    QByteArray out = process->readAllStandardOutput();
    QByteArray error = process->readAllStandardError();
    for (auto &&arr : {error, out}) {
      QString output_str(QString::fromUtf16(
          reinterpret_cast<const char16_t *>(arr.data()), arr.size() / 2));
      output->setText(output->toPlainText().append(output_str));
      QTextCursor cursor = output->textCursor();
      cursor.movePosition(QTextCursor::End);
      output->setTextCursor(cursor);
    }
  });

  QObject::connect(process,
                   static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                       &QProcess::finished),
                   [start, process, output](int, QProcess::ExitStatus) {
                     output->clear();
                     start(process);
                   });
}

inline static void disconectProcessFromAll(QProcess *process) {
  process->disconnect();
}

inline QString getSaveFileName(QWidget *parent, const QString &filter,
                               const QString &selected_filter) {
  auto &&file_name = QFileDialog::getSaveFileName(parent, "Сохранение",
                                                  filter, selected_filter);
  if (file_name.isEmpty()) {
    QMessageBox::warning(parent, "Внимание", "Файл не был выбран");
    file_name.clear();
    return file_name;
  }
  return file_name;
}

inline QString getOpenFileName(QWidget *parent, const QString &filter,
                               const QString &selected_filter) {
  auto &&file_name = QFileDialog::getOpenFileName(parent, "Открытие",
                                                  filter, selected_filter);
  if (file_name.size() == 0) {
    QMessageBox::warning(parent, "Внимание", "Файл не был выбран");
    file_name.clear();
    return file_name;
  }
  return file_name;
}

inline bool getXMLFromFile(QWidget *parent, const QString &file_name,
                           QTreeView *tree_view) {
  QFile file(file_name);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(parent, "Ошибка",
                          "Не удалось открыть файл " + file_name);
    return false;
  }

  QStandardItemModel *model = new QStandardItemModel(0, 0, tree_view);
  tree_view->setModel(model);
  QStandardItem *parent_item = model->invisibleRootItem();

  QXmlStreamReader xml_reader(&file);
  QStack<QStandardItem *> tags;
  tags.push(parent_item);
  while (!xml_reader.atEnd()) {
    xml_reader.readNext();
    switch (xml_reader.tokenType()) {
    case QXmlStreamReader::StartElement: { // открывающий тэг
      auto table_model = new QStandardItemModel();
      for (auto &attr : xml_reader.attributes()) {
        auto attr_name_item = new QStandardItem(attr.name().toString());
        auto attr_value_item = new QStandardItem(attr.value().toString());
        table_model->appendRow(
            QList<QStandardItem *>({attr_name_item, attr_value_item}));
      }
      table_model->setHorizontalHeaderLabels(
          QStringList({"Attribute", "Value"}));
      auto &&new_tag = createTag(table_model, xml_reader.name().toString());
      auto &&parent_tag = tags.top();
      parent_tag->appendRow(new_tag);
      tags.push(new_tag);
      break;
    }
    case QXmlStreamReader::Characters: { // текст внутри тэга
      QString str = xml_reader.text().toString().trimmed();
      if (!str.isEmpty()) {
        auto tag = tags.top();
        tag->setText(tag->text() + ": " + str);
      }
      break;
    }
    case QXmlStreamReader::EndElement: {
      tags.pop();
      break;
    }
    case QXmlStreamReader::StartDocument:
    case QXmlStreamReader::EndDocument:
      break;
    default: {
      qDebug() << "error: unexpected token type " << xml_reader.tokenType();
      break;
    }
    }
  }
  file.close();
  return true;
}

inline void writeTagsFromModel(QXmlStreamWriter &xml_writer,
                               const QStandardItemModel *model);

inline bool saveXMLToFile(QWidget *parent, const QString &file_name,
                          const QStandardItemModel *model) {
  QFile file(file_name);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(parent, "Ошибка",
                          "Не удалось сохранить в файл " + file_name);
    return false;
  }

  QXmlStreamWriter xml_writer(&file);
  xml_writer.setAutoFormatting(true);

  xml_writer.writeStartDocument();
  writeTagsFromModel(xml_writer, model);
  xml_writer.writeEndDocument();
  file.close();
  QMessageBox::information(parent,"Сохранение", "Файл успешно сохранён в " + file_name);
  return true;
}

inline bool getYAMLFromFile(QWidget *parent, const QString &file_name,
                            BlockFieldWidget *field_widget) {
  QFile file(file_name);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(parent, "Ошибка",
                          "Не удалось открыть файл " + file_name);
    return false;
  }

  file.close();
  return true;
}

inline bool saveYAMLToFile(QWidget *parent, const QString &file_name) {
  QFile file(file_name);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(parent, "Ошибка",
                          "Не удалось сохранить в файл " + file_name);
    return false;
  }

  file.close();
  QMessageBox::information(parent, "Сохранение",
                           "Файл успешно сохранён в " + file_name);
  return true;
}

static inline void writeAttributesFromModel(QXmlStreamWriter &xml_writer,
                                            const QStandardItemModel *model) {
  for (int r = 0; r < model->rowCount(); ++r) {
    auto &&name = model->item(r, 0)->text();
    auto &&value = model->item(r, 1)->text();
    xml_writer.writeAttribute(name, value);
  }
}

static inline void writeTags(QXmlStreamWriter &xml_writer,
                             const QStandardItem *root) {
  if (!root)
    return;
  auto &&tag_data = root->text().split(": ");
  xml_writer.writeStartElement(tag_data.front());
  auto &&table_model =
      root->data(Qt::UserRole + 1).value<QStandardItemModel *>();
  writeAttributesFromModel(xml_writer, table_model);
  if (tag_data.size() > 1) {
    tag_data.pop_front();
    xml_writer.writeCharacters(tag_data.join(": "));
  } else {
    for (int r = 0; root->hasChildren() && r < root->rowCount(); ++r) {
      auto &&item = root->child(r);
      writeTags(xml_writer, item);
    }
  }
  xml_writer.writeEndElement();
}

inline void writeTagsFromModel(QXmlStreamWriter &xml_writer,
                               const QStandardItemModel *model) {
  writeTags(xml_writer, model->item(0, 0));
}

inline void setDisableForButtonsInLayout(QLayout *layout, bool is_disabled) {
  for (int i = 0; i < layout->count(); ++i) {
    if (auto &&button =
            qobject_cast<QPushButton *>(layout->itemAt(i)->widget()))
      button->setDisabled(is_disabled);
  }
}
