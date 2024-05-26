#include "defaultcontroller.h"
#include "../blockfield.h"

#include <QDebug>

static bool isPointOnLine(QLine line, QPoint point) {
  float x1 = line.p1().x(), y1 = line.p1().y();
  float x2 = line.p2().x(), y2 = line.p2().y();
  float k = (y2 - y1) / (x2 - x1);
  float b = y1 - k * x1;
  float eps = 6;
  return (abs(point.y() - (k * point.x() + b)) < eps);
}

DefaultController::DefaultController(FieldModel &field_model,
                                     SelectionModel &selection_model,
                                     LineModel &line_model)
    : _field_model(field_model), _selection_model(selection_model),
      _line_model(line_model) {}

void DefaultController::onMouseMoveEvent(QWidget *widget, QMouseEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    block_w->GetLeftNode()->makeTransparent(false);
    block_w->GetRightNode()->makeTransparent(false);
    if (event->buttons() == Qt::LeftButton && _old_block_pos) {
      QPoint delta = event->pos() - *_old_block_pos;
      block_w->move(block_w->pos() + delta);
      block_w->parentWidget()->repaint();
    }
  }
}

void DefaultController::onMousePressEvent(QWidget *widget, QMouseEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->button() == Qt::LeftButton) {
      _old_block_pos = event->pos();
      block_w->parentWidget()->repaint();
    }
  } else if (auto &&field_w = qobject_cast<BlockField *>(widget)) {
    auto &&_connection_map = _field_model.GetConnectionMap();
    auto &&_map_of_selected_nodes = _selection_model.GetSelectionMap();
    for (auto it = _connection_map.begin(); it != _connection_map.end(); ++it) {
      auto start = it.key();
      auto start_pos = start->coordToParent();
      for (auto end_node_it = it.value().begin();
           end_node_it != it.value().end(); ++end_node_it) {
        auto end = *end_node_it;
        auto end_pos = end->coordToParent();
        bool find_line = isPointOnLine(QLine(start_pos, end_pos), event->pos());
        if (find_line) {
          _selection_model.AddSelection(start, end);
          return;
        }
      }
    }
    _selection_model.Clear();
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    _line_model.SetBegin(connect_node_w);
  }
}

void DefaultController::onKeyPressEvent(QWidget *widget, QKeyEvent *event) {
  if (auto &&field_w = qobject_cast<BlockField *>(widget)) {
    auto &&_connection_map = _field_model.GetConnectionMap();
    auto &&_map_of_selected_nodes = _selection_model.GetSelectionMap();
    if (event->key() == Qt::Key::Key_Delete) {
      for (auto start_node = _map_of_selected_nodes.begin();
           start_node != _map_of_selected_nodes.end(); ++start_node) {
        for (auto end_node_it = start_node.value().begin();
             end_node_it != start_node.value().end(); ++end_node_it) {
          _field_model.RemoveConnection(start_node.key(), *end_node_it);
        }
        if (start_node.value().empty())
          _field_model.Remove(start_node.key());
      }
      _selection_model.Clear();
      // repaint();
    }
  }
}

void DefaultController::onLeaveEvent(QWidget *widget, QEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    block_w->GetLeftNode()->makeTransparent(true);
    block_w->GetRightNode()->makeTransparent(true);
  }
}

void DefaultController::onMouseReleaseEvent(QWidget *widget,
                                            QMouseEvent *event) {
  _old_block_pos = std::nullopt;
}
