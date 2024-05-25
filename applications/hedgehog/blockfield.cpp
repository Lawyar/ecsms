#include "blockfield.h"
#include "connectnodewidget.h"
#include "controlls/defaultcontroller.h"
#include "controlls/drawlinecontroller.h"
#include "events/changecontrollerevent.h"
#include "events/drawevent.h"
#include "events/drawlineevent.h"
#include "events/mypaintevent.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

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
    repaint();
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

void BlockField::mouseMoveEvent(QMouseEvent *event) {
  _controller->onMouseMoveEvent(event);
}

void BlockField::mousePressEvent(QMouseEvent *event) {
  _controller->onMousePressEvent(event);
}

void BlockField::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.eraseRect(rect());
  p.setBackground(QBrush(Qt::white));

  if (auto && begin = _line_model.GetBegin())
  {
    auto &&point1 = begin->coordToParent();
    auto &&point2 = _line_model.GetEnd();
    p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    p.drawLine(point1, point2);
  }

  /*if (auto &&my_paint_ev = dynamic_cast<MyPaintEvent *>(event)) {
    auto &&drawEvent = my_paint_ev->GetEvent();
    switch (drawEvent->GetDrawEventType()) {
    case DrawEventType::Line: {
      auto drawLineEvent = static_cast<DrawLineEvent *>(drawEvent.get());
      
      break;
    }
    default: {
      assert(false);
      break;
    }
    }
  }

  /*QVector<QLineF> unselected_lines, selected_lines;
  for (auto it = _connection_map.begin(); it != _connection_map.end(); ++it) {
    auto start = it.key();
    auto start_pos = start->pos; // coordToBlockField(start);
    for (auto end_node_it = it.value().begin(); end_node_it != it.value().end();
         ++end_node_it) {
      auto end = *end_node_it;
      auto end_pos = end->pos; // coordToBlockField(end);
      start->makeTransparent(false);
      end->makeTransparent(false);

      if (_map_of_selected_nodes.value(start).contains(end)) {
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

  if (!selected_node)
    return;

  selected_node->makeTransparent(false);
  p.drawLine(coordToBlockField(selected_node), _pos);*/
}

void BlockField::keyPressEvent(QKeyEvent *event) {
  _controller->onKeyPressEvent(event);
}

void BlockField::on_start(ConnectNodeWidget *start) {
  _controller->on_start(start);
}
