#include "blockwidget.h"
#include "blockfieldwidget.h"
#include "connectnodewidget.h"
#include "../models/nodetype.h"
#include <QFontMetrics>

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

QFont BlockWidget::_font("times", 12);

BlockWidget::BlockWidget(const BlockId &id,
                         std::unique_ptr<IController> &controller,
                         const QString &text,
                         BlockFieldWidget *parent)
    : _id(id), _controller(controller), QWidget(parent),
      _block_name(new QLabel(text, this)),

      _left_node(new ConnectNodeWidget(
          id.GetChildId(static_cast<PartId>(NodeType::Incoming)), controller,
          NodeType::Incoming, this)),

      _right_node(new ConnectNodeWidget(
          id.GetChildId(static_cast<PartId>(NodeType::Outgoing)), controller,
          NodeType::Outgoing, this)),

      _resume_pause_button(new QPushButton("||", this)) {
  _block_name->setWordWrap(true);
  _block_name->setAlignment(Qt::AlignCenter);
  _block_name->setFrameStyle(QFrame::Box | QFrame::Plain);
  _block_name->setFont(_font);
  _resume_pause_button->setFont(_font);

  _block_name->show();
  auto &&rect = _resume_pause_button->rect();
  rect.setHeight(_button_height);
  _resume_pause_button->setGeometry(rect);
  _resume_pause_button->show();

  auto label_width = _block_name->width();
  auto label_height = _block_name->height();
  auto _button_width = _resume_pause_button->width();
  auto diameter = _nodes_radius * 2;

  auto width = 2 * (2 * _spacing1 + diameter) + label_width;
  auto height = 2 * _spacing1 + _button_height + _spacing2 + label_height;
  setGeometry(0, 0, width, height);

  _resume_pause_button->move((width - _button_width) / 2, _spacing1);
  _block_name->move(2 * _spacing1 + diameter,
                    _spacing1 + _button_height + _spacing2);

  _left_node->move(_spacing1, _spacing1 + _button_height + _spacing2 +
                                  label_height / 2 - diameter / 2);
  _right_node->move(width - _spacing1 - diameter,
                    _spacing1 + _button_height + _spacing2 + label_height / 2 -
                        diameter / 2);

  setMouseTracking(true);

  connect(_resume_pause_button, &QPushButton::clicked, this,
          &BlockWidget::on_pushButton_clicked);
}

BlockId BlockWidget::GetId() const { return _id; }

QWidget *BlockWidget::FindById(Id id) {
  ConnectNodeWidget *res = nullptr;
  for (auto &&node : {_left_node, _right_node}) {
    if (node->GetId() == id) {
      res = node;
      break;
    }
  }
  return res;
}

ConnectNodeWidget *BlockWidget::GetLeftNode() { return _left_node; }

ConnectNodeWidget *BlockWidget::GetRightNode() { return _right_node; }

QPoint BlockWidget::CoordToBlockField(QPoint p) const { return mapToParent(p); }

QSize BlockWidget::GetSize(const QString &text) {
  QString labelText = prepareStringForLabel(text);
  QFontMetrics metrics(_font);
  QRect rect = metrics.boundingRect(labelText);
  return {rect.width(), rect.height()};
}

QPoint BlockWidget::GetLeftNodeOffset(const QString &text) {
  QString labelText = prepareStringForLabel(text);
  QFontMetrics metrics(_font);
  QRect rect = metrics.boundingRect(labelText);

  QPoint res(_spacing1 + _nodes_radius, _spacing1 + _button_height + _spacing2 + rect.height() / 2);
  return res;
}

QPoint BlockWidget::GetRightNodeOffset(const QString &text) {
  QString labelText = prepareStringForLabel(text);
  QFontMetrics metrics(_font);
  QRect rect = metrics.boundingRect(labelText);

  QPoint res((_spacing1 + _nodes_radius) * 3 + rect.width(),
    _spacing1 + _button_height + _spacing2 + rect.height() / 2);
  return res;
}

int BlockWidget::GetNodesRadius() { return _nodes_radius; }

void BlockWidget::SetText(QString text) { _block_name->setText(text); }

void BlockWidget::mouseMoveEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    setCursor(Qt::CursorShape::ClosedHandCursor);
  _controller->onMouseMoveEvent(this, event);
}

void BlockWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    setCursor(Qt::CursorShape::ClosedHandCursor);
  _controller->onMousePressEvent(this, event);
}

void BlockWidget::keyPressEvent(QKeyEvent *event) {
  _controller->onKeyPressEvent(this, event);
}

void BlockWidget::enterEvent(QEvent *event) {
  _controller->onEnterEvent(this, event);
}

void BlockWidget::leaveEvent(QEvent *event) {
  _controller->onLeaveEvent(this, event);
}

void BlockWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    setCursor(Qt::CursorShape::OpenHandCursor);
  _controller->onMouseReleaseEvent(this, event);
}

void BlockWidget::on_pushButton_clicked() {
  auto text = _resume_pause_button->text();
  if (text == "||")
    _resume_pause_button->setText("▶");
  else
    _resume_pause_button->setText("||");
}

QString BlockWidget::prepareStringForLabel(const QString &str) { return str; }
