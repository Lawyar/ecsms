#ifndef QLINEEDITDELEGATE_H
#define QLINEEDITDELEGATE_H

#include <QItemDelegate>
#include <QObject>
#include <QRegExpValidator>
#include <QRegExp>
#include <QLineEdit>

class QLineEditDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    QLineEditDelegate(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData( QWidget           * Editor ,
                        const QModelIndex & Index  ) const;

    void setModelData( QWidget            * Editor ,
                       QAbstractItemModel * Model  ,
                       const QModelIndex  & Index  ) const;

    void updateEditorGeometry( QWidget                    * Editor ,
                               const QStyleOptionViewItem & Option ,
                               const QModelIndex          & Index  ) const;
};

#endif // QLINEEDITDELEGATE_H
