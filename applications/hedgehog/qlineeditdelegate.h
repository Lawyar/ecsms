#ifndef QLINEEDITDELEGATE_H
#define QLINEEDITDELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include <QObject>
#include <QRegExp>
#include <QRegExpValidator>

enum class WhatValidate { XMLTag, XMLAttribute };

class QLineEditDelegate : public QItemDelegate {
  Q_OBJECT

public:
  QLineEditDelegate(QObject *parent, WhatValidate type);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

  void setEditorData(QWidget *Editor, const QModelIndex &Index) const;

  void setModelData(QWidget *Editor, QAbstractItemModel *Model,
                    const QModelIndex &Index) const;

  void updateEditorGeometry(QWidget *Editor, const QStyleOptionViewItem &Option,
                            const QModelIndex &Index) const;

private:
  WhatValidate _type;
};

#endif // QLINEEDITDELEGATE_H
