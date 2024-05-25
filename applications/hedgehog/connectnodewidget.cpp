#include "connectnodewidget.h"

#include <QDebug>
#include <QPainter>

ConnectNodeWidget::ConnectNodeWidget(NodeType type, QWidget *parent)
    : QLabel(parent), _type(type) {
  QPixmap pixmap(11, 11);
  setPixmap(pixmap);
  makeTransparent(true);
  setMouseTracking(true);
}

NodeType ConnectNodeWidget::getNodeType() const { return _type; }

void ConnectNodeWidget::makeTransparent(bool is_transparent) {
  if (is_transparent) {
    QPixmap pm(*pixmap());
    pm.fill(Qt::transparent);
    setPixmap(pm);
  } else {
    QPixmap pm(*pixmap());
    QPainter painter(&pm);
    painter.setBrush(QBrush(Qt::green));
    auto radius = 5;
    auto point = QPoint(radius, radius);
    painter.drawEllipse(point, radius, radius);
    setPixmap(pm);
  }
}

QPoint ConnectNodeWidget::coordToParent() const {
  auto parent = parentWidget();
  return parent->mapToParent(pos()) + QPoint(width() / 2, height() / 2);
}

void ConnectNodeWidget::mousePressEvent(QMouseEvent *event) {
  emit start(this);
  qDebug() << "start from node";
}
