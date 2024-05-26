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
                                     LineModel &line_model,
                                     ActiveNodesModel &active_nodes)
    : _field_model(field_model), _selection_model(selection_model),
      _line_model(line_model), _active_nodes_model(active_nodes) {}

void DefaultController::onMouseMoveEvent(QWidget *widget, QMouseEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->buttons() == Qt::LeftButton && _old_block_pos) {
      QPoint delta = event->pos() - *_old_block_pos;
      block_w->move(block_w->pos() + delta);
      block_w->parentWidget()->repaint();
    }
  }
}

void DefaultController::onMousePressEvent(QWidget *widget, QMouseEvent *event) {
  if (auto &&field_w = qobject_cast<BlockField *>(widget)) {
    auto &&_connection_map = _field_model.GetConnectionMap();
    auto &&_map_of_selected_nodes = _selection_model.GetSelectionMap();
    for (auto it = _connection_map.begin(); it != _connection_map.end(); ++it) {
      auto start = it.key();
      auto start_pos = start->getCenterCoordToBlockField();
      for (auto end_node_it = it.value().begin();
           end_node_it != it.value().end(); ++end_node_it) {
        auto end = *end_node_it;
        auto end_pos = end->getCenterCoordToBlockField();
        bool find_line = isPointOnLine(QLine(start_pos, end_pos), event->pos());
        if (find_line) {
          _selection_model.AddSelection(start, end);
          return;
        }
      }
    }
    _selection_model.Clear();
    qDebug() << "remove lines and blocks from selection";
  } else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->button() == Qt::LeftButton) {
      _old_block_pos = event->pos();
      qDebug() << "add block to selection";
      _selection_model.AddSelection(block_w);
    }
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
      qDebug() << "deleting selected connections and blocks";
      // delete connections with map
      for (auto &&start_node : _map_of_selected_nodes.keys()) {
        for (auto &&end_node : _map_of_selected_nodes[start_node]) {
          if (end_node) {
            _field_model.RemoveConnection(start_node, end_node);
            _active_nodes_model.DecreaseNodeCount(start_node);
            _active_nodes_model.DecreaseNodeCount(end_node);
          }
        }
      }
      // delete blocks
      for (auto &&block : _selection_model.GetSelectedBlocks()) {
        _field_model.RemoveBlock(block);
      }
      _selection_model.Clear();
    }
  }
}

void DefaultController::onEnterEvent(QWidget *widget, QEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _active_nodes_lock.reset(
        new ActiveNodesLock(_active_nodes_model,
                            {block_w->GetLeftNode(), block_w->GetRightNode()}));
  }
}

void DefaultController::onLeaveEvent(QWidget *widget, QEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _active_nodes_lock.reset();
  }
}

void DefaultController::onMouseReleaseEvent(QWidget *widget,
                                            QMouseEvent *event) {
  _old_block_pos = std::nullopt;
}
