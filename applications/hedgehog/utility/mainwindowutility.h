#pragma once

#include <QLayout>
#include <QPushButton>
#include <QXmlStreamWriter>

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
