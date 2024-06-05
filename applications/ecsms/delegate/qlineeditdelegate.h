#ifndef QLINEEDITDELEGATE_H
#define QLINEEDITDELEGATE_H

#include "../controlls/command/commandmanager.h"

#include <QTreeView>
#include <QItemDelegate>
#include <QLineEdit>
#include <QObject>
#include <QRegExp>
#include <QRegExpValidator>
#include <memory>

enum class WhatValidate { XMLTag, XMLAttribute, Nothing };

class QLineEditDelegate : public QItemDelegate {
  Q_OBJECT

public:
  QLineEditDelegate(QObject *parent, WhatValidate type,
                    std::shared_ptr<CommandManager> cm, QTreeView *tree_view);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;

  void setEditorData(QWidget *Editor, const QModelIndex &Index) const;

  void setModelData(QWidget *Editor, QAbstractItemModel *Model,
                    const QModelIndex &Index) const;

  void updateEditorGeometry(QWidget *Editor, const QStyleOptionViewItem &Option,
                            const QModelIndex &Index) const;

private:
  std::shared_ptr<CommandManager> _cm;
  WhatValidate _type;
  QTreeView *_tree_view;
};

#endif // QLINEEDITDELEGATE_H
