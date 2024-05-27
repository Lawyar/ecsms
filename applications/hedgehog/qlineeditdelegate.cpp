#include "qlineeditdelegate.h"

#include <QDebug>
#include <QToolTip>

static bool isXMLTagName(const QString &text, bool has_child_element,
                         QString *err_text = nullptr) {
  bool res = false;
  bool starts_with_xml = QRegExp("^[xX][mM][lL].*").indexIn(text) == 0;
  bool correct_begining = QRegExp("^[a-zA-Z_]+").indexIn(text) == 0;

  if (starts_with_xml) {
    if (err_text)
      *err_text = "имя тэга не должно начинаться с xml (в любом регистре)";
    return res;
  }

  if (!correct_begining) {
    if (err_text)
      *err_text = "имя тэга должно начинаться с _ или буквы";
    return res;
  }

  bool correct_body;
  if (has_child_element) {
    correct_body = QRegExp("^[a-zA-Z_]+[\\w\\-\\.]*$").indexIn(text) == 0;
    if (!correct_body) {
      if (err_text)
        *err_text = "данный тэг может содержать только элементный контент";
      return res;
    }
  } else {
    correct_body =
        QRegExp("^[a-zA-Z_]+[\\w\\-\\.]*(: .+)?$").indexIn(text) == 0;
    if (!correct_body) {
      if (err_text)
        *err_text = "<имя тэга>: <текст>";
      return res;
    }
  }
  return true;
}

static bool isXMLAttributeName(const QString &text,
                               QString *err_text = nullptr) {
  bool res = false;
  bool correct_begining = QRegExp("^[a-zA-Z_]+").indexIn(text) == 0;

  if (!correct_begining) {
    if (err_text)
      *err_text = "имя атрибута должно начинаться с _ или буквы";
    return res;
  }

  bool correct_body = QRegExp("^[a-zA-Z_]+[\\w\\-\\.]*$").indexIn(text) == 0;
  if (!correct_body) {
    if (err_text)
      *err_text = "имя атрибута может содержать только цифры, буквы, _, . и -";
    return res;
  }

  return true;
}

class XMLTagValidator : public QValidator {
public:
  XMLTagValidator(const QModelIndex &index) : _index(index) {}

  virtual QValidator::State validate(QString &text, int &pos) const override {
    if (text.isEmpty())
      return Intermediate;
    auto res = isXMLTagName(text, _index.child(0, 0) != QModelIndex())
                   ? Acceptable
                   : Intermediate;
    return res;
  }

private:
  QModelIndex _index;
};

class XMLAttributeValidator : public QValidator {
public:
  XMLAttributeValidator(const QModelIndex &index) : _index(index) {}

  virtual QValidator::State validate(QString &text, int &pos) const override {
    if (text.isEmpty())
      return Intermediate;
    auto res = isXMLAttributeName(text) ? Acceptable : Intermediate;
    return res;
  }

private:
  QModelIndex _index;
};

QLineEditDelegate::QLineEditDelegate(QObject *parent, WhatValidate type)
    : QItemDelegate(parent), _type(type) {}

QWidget *QLineEditDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const {
  QValidator *validator;
  switch (_type) {
  case WhatValidate::XMLTag: {
    validator = new XMLTagValidator(index);
    break;
  }
  case WhatValidate::XMLAttribute: {
    validator = new XMLAttributeValidator(index);
    break;
  }
  default: {
    assert(false);
    return nullptr;
  }
  }

  auto editor = new QLineEdit(parent);
  QObject::connect(
      editor, &QLineEdit::textEdited,
      [type = _type, editor, index](const QString &text) {
        QString err;
        switch (type) {
        case WhatValidate::XMLTag: {
          if (!isXMLTagName(text, index.child(0, 0) != QModelIndex(), &err)) {
            QToolTip::showText(editor->mapToGlobal(QPoint()), err);
          }
          break;
        }
        case WhatValidate::XMLAttribute: {
          if (!isXMLAttributeName(text, &err)) {
            QToolTip::showText(editor->mapToGlobal(QPoint()), err);
          }
          break;
        }
        default: {
          assert(false);
          return;
        }
        }
      });
  editor->setValidator(validator);

  return editor;
}

void QLineEditDelegate::setEditorData(QWidget *Editor,
                                      const QModelIndex &Index) const {
  auto Value = Index.model()->data(Index, Qt::EditRole);
  auto CastedEditor = static_cast<QLineEdit *>(Editor);
  CastedEditor->setText(Value.toString());
}

void QLineEditDelegate::setModelData(QWidget *Editor, QAbstractItemModel *Model,
                                     const QModelIndex &Index) const {
  auto CastedEditor = static_cast<QLineEdit *>(Editor);
  auto Value = CastedEditor->text();
  Model->setData(Index, Value, Qt::EditRole);
}

void QLineEditDelegate::updateEditorGeometry(QWidget *Editor,
                                             const QStyleOptionViewItem &Option,
                                             const QModelIndex &Index) const {
  Editor->setGeometry(Option.rect);
}
