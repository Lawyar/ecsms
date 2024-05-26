#include "blockfield.h"
#include "connectnodewidget.h"
#include "controlls/defaultcontroller.h"
#include "controlls/drawlinecontroller.h"
#include "events/changeactivenodeevent.h"
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
  _controller.reset(new DefaultController(_field_model, _selection_model,
                                          _line_model, _active_nodes_model));
  _field_model.Subscribe(this);
  _selection_model.Subscribe(this);
  _line_model.Subscribe(this);
  _active_nodes_model.Subscribe(this);
}

void BlockField::AddNewBlock() {
  auto default_block = new BlockWidget(_controller, this);
  _field_model.AddBlock(default_block);
  default_block->show();
  default_block->move(rect().center());
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
      _controller.reset(new DrawLineController(_field_model, _line_model,
                                               _active_nodes_model));
      break;
    }
    case defaultController: {
      _controller.reset(new DefaultController(
          _field_model, _selection_model, _line_model, _active_nodes_model));
      break;
    }
    default: {
      assert(false);
      break;
    }
    }
    break;
  }
  case changeActiveNodeEvent: {
    auto &&change_e = std::static_pointer_cast<ChangeActiveNodeEvent>(e);
    auto &&node = change_e->GetNode();
    node->makeTransparent(!change_e->GetActivity());
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

void BlockField::enterEvent(QEvent *event) { _controller->onEnterEvent(this, event); }

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

  /*-DRAW SELECTED AND NOT SELECTED LINES-*/
  QVector<QLineF> unselected_lines, selected_lines;
  auto &&_connection_map = _field_model.GetConnectionMap();
  for (auto it = _connection_map.begin(); it != _connection_map.end(); ++it) {
    auto start = it.key();
    for (auto end_node_it = it.value().begin(); end_node_it != it.value().end();
         ++end_node_it) {
      auto end = *end_node_it;

      auto start_pos = start->getCenterCoordToBlockField();
      auto end_pos = end->getCenterCoordToBlockField();
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
  /*-------------------------------------*/

  /*-DRAW FRAME FOR SELECTED BLOCKS-*/
  p.setPen(QPen(Qt::green, 1, Qt::SolidLine));
  for (auto &&selected_block : _selection_model.GetSelectedBlocks()) {
    auto &&rect = selected_block->rect();
    auto &&mapped_rect =
        QRect(selected_block->mapToParent({rect.x(), rect.y()}), QSize({rect.width(), rect.height()}));
    p.drawRect(mapped_rect);
  }
  /*--------------------------------*/

  if (auto &&begin = _line_model.GetBegin()) { // draw connection line
    auto &&point1 = begin->getCenterCoordToBlockField();
    auto &&point2 = _line_model.GetEnd();
    p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    p.drawLine(point1, point2);
  }
}
