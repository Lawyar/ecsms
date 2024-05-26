#include "connectnodewidget.h"

#include <QDebug>
#include <QPainter>

ConnectNodeWidget::ConnectNodeWidget(std::unique_ptr<IController> &controller,
                                     NodeType type, QWidget *parent)
    : QLabel(parent), _controller(controller), _type(type) {
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

QPoint ConnectNodeWidget::coordToBlockField(QPoint pos) const {
  auto pos_to_parent = mapToParent(pos);
  auto parent = parentWidget();
  return parent->mapToParent(pos_to_parent);
}

QPoint ConnectNodeWidget::getCenterCoordToBlockField() const {
  auto parent = parentWidget();
  return parent->mapToParent(pos()) + QPoint(width() / 2, height() / 2);
}

void ConnectNodeWidget::mouseMoveEvent(QMouseEvent *event) {
  _controller->onMouseMoveEvent(this, event);
}

void ConnectNodeWidget::mousePressEvent(QMouseEvent *event) {
  _controller->onMousePressEvent(this, event);
}

void ConnectNodeWidget::keyPressEvent(QKeyEvent *event) {
  _controller->onKeyPressEvent(this, event);
}

void ConnectNodeWidget::enterEvent(QEvent *event) {
  _controller->onEnterEvent(this, event);
}

void ConnectNodeWidget::leaveEvent(QEvent *event) {
  _controller->onLeaveEvent(this, event);
}

void ConnectNodeWidget::mouseReleaseEvent(QMouseEvent *event) {
  _controller->onMouseReleaseEvent(this, event);
}
