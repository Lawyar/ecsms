#include "tagtextchangedcommand.h"

#include <QDebug>
#include <qabstractitemmodel.h>

TextChangedCommand::TextChangedCommand(QModelIndex tag, QString old_text,
                                             QString new_text)
    : _old_text(old_text), _new_text(new_text) {
  _row = tag.row();
  _col = tag.column();
  _model = qobject_cast<const QStandardItemModel *>(tag.model());
  _rows.insert(_rows.begin(), tag.row());
  auto &&parent_tag_index = tag.parent();
  while (parent_tag_index != _model->invisibleRootItem()->index()) {
    _rows.insert(_rows.begin(), parent_tag_index.row());
    parent_tag_index = parent_tag_index.parent();
  }
}

void TextChangedCommand::Execute() {
  QStandardItem *parent_tag = _model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, _col);
  }
  parent_tag->child(_row, _col)->setText(_new_text);
}

void TextChangedCommand::UnExecute() {
  QStandardItem *parent_tag = _model->invisibleRootItem();
  for (int row : _rows) {
    parent_tag = parent_tag->child(row, _col);
  }
  parent_tag->child(_row, _col)->setText(_old_text);
}
