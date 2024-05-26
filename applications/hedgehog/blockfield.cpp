#include "blockfield.h"
#include "connectnodewidget.h"
#include "controlls/defaultcontroller.h"
#include "controlls/drawlinecontroller.h"
#include "events/changecontrollerevent.h"
#include "events/drawevent.h"
#include "events/mypaintevent.h"
#include "events/repaintevent.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <set>

BlockField::BlockField(QWidget *parent) : QWidget(parent) {
  setMouseTracking(true);
  setFocus(Qt::FocusReason::ActiveWindowFocusReason);
  _controller.reset(
      new DefaultController(_field_model, _selection_model, _line_model));
  _field_model.Subscribe(this);
  _selection_model.Subscribe(this);
  _line_model.Subscribe(this);
}

void BlockField::Update(std::shared_ptr<Event> e) {
  switch (e->GetEventType()) {
  case drawEvent: {
    auto &&draw_e = std::static_pointer_cast<DrawEvent>(e);
    switch (draw_e->GetDrawEventType()) {
    case repaintEvent: {
      repaint();
      break;
    }
    default: {
      assert(false);
      break;
    }
    }
    break;
  }
  case changeControllerEvent: {
    auto &&change_ctr_e = std::static_pointer_cast<ChangeControllerEvent>(e);
    switch (change_ctr_e->GetControllerType()) {
    case drawLineController: {
      _controller.reset(new DrawLineController(_field_model, _line_model));
      break;
    }
    case defaultController: {
      _controller.reset(
          new DefaultController(_field_model, _selection_model, _line_model));
      break;
    }
    default: {
      assert(false);
      break;
    }
    }
    break;
  }
  default: {
    assert(false);
    break;
  }
  }
}

std::unique_ptr<IController> &BlockField::GetController() {
  return _controller;
}

void BlockField::mouseMoveEvent(QMouseEvent *event) {
  _controller->onMouseMoveEvent(this, event);
}

void BlockField::mousePressEvent(QMouseEvent *event) {
  _controller->onMousePressEvent(this, event);
}

void BlockField::keyPressEvent(QKeyEvent *event) {
  _controller->onKeyPressEvent(this, event);
}

void BlockField::leaveEvent(QEvent *event) {
  _controller->onLeaveEvent(this, event);
}

void BlockField::mouseReleaseEvent(QMouseEvent *event) {
  _controller->onMouseReleaseEvent(this, event);
}

void BlockField::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.eraseRect(rect());
  p.setBackground(QBrush(Qt::white));

  QVector<QLineF> unselected_lines, selected_lines;
  auto &&_connection_map = _field_model.GetConnectionMap();
  for (auto it = _connection_map.begin(); it != _connection_map.end(); ++it) {
    auto start = it.key();
    for (auto end_node_it = it.value().begin(); end_node_it != it.value().end();
         ++end_node_it) {
      auto end = *end_node_it;

      auto start_pos = start->coordToParent();
      auto end_pos = end->coordToParent();
      auto &&connect_vec = _selection_model.GetSelectionMap().value(start);
      if (connect_vec.contains(end)) {
        selected_lines.append({start_pos, end_pos});
      } else {
        unselected_lines.append({start_pos, end_pos});
      }
    }
  }

  p.setPen(QPen(Qt::green, 1, Qt::SolidLine));
  p.drawLines(selected_lines.data(), selected_lines.size());

  p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
  p.drawLines(unselected_lines.data(), unselected_lines.size());

  if (auto &&begin = _line_model.GetBegin()) { // draw connection line
    begin->makeTransparent(false);
    auto &&point1 = begin->coordToParent();
    auto &&point2 = _line_model.GetEnd();
    p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    p.drawLine(point1, point2);
  }
}
