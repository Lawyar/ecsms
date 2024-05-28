#include "defaultcontroller.h"
#include "../blockfield.h"
#include "command/removecommand.h"

#include <QDebug>

static float distance(QPoint p1, QPoint p2) {
  float d = sqrt(pow((p1.x() - p2.x()), 2) + pow((p1.y() - p2.y()), 2));
  return d;
}

static bool isPointOnLine(QLine line, QPoint point) {
  float x1 = line.p1().x(), y1 = line.p1().y();
  float x2 = line.p2().x(), y2 = line.p2().y();
  float k = (y2 - y1) / (x2 - x1);
  float b = y1 - k * x1;
  float eps = 6;
  bool res = (distance(line.p1(), point) + distance(line.p2(), point) -
              distance(line.p1(), line.p2())) < eps;
  return res && (abs(point.y() - (k * point.x() + b)) < eps);
}

DefaultController::DefaultController(FieldModel &field_model,
                                     SelectionModel &selection_model,
                                     LineModel &line_model,
                                     ActiveNodesModel &active_nodes,
                                     CommandManager &cm)
    : _field_model(field_model), _selection_model(selection_model),
      _line_model(line_model), _active_nodes_model(active_nodes), _cm(cm) {}

void DefaultController::onMouseMoveEvent(QWidget *widget, QMouseEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->buttons() == Qt::LeftButton && _old_block_pos) {
      QPoint delta = event->pos() - *_old_block_pos;
      block_w->move(block_w->pos() + delta);
      FieldModel::BlockData block_data = {block_w->pos()};
      _field_model.SetBlockData(block_w->GetId(), block_data);
      block_w->parentWidget()->repaint();
    }
  }
}

void DefaultController::onMousePressEvent(QWidget *widget, QMouseEvent *event) {
  if (qobject_cast<BlockField *>(widget)) {
    onFieldMousePress(event);
  } else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->button() == Qt::LeftButton) {
      _old_block_pos = event->pos();
      _selection_model.AddSelection(block_w->GetId());
    }
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    _active_nodes_model.SetBeginOfLine(connect_node_w->GetId());
    auto &&node_center = connect_node_w->getCenterCoordToBlockField();
    _line_model.SetBegin(node_center);
  }
}

void DefaultController::onKeyPressEvent(QWidget *widget, QKeyEvent *event) {
  if (qobject_cast<BlockField *>(widget)) {
    onFieldKeyPress(event);
  }
}

void DefaultController::onEnterEvent(QWidget *widget, QEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _active_nodes_lock.reset(new ActiveNodesLock(
        _active_nodes_model,
        {block_w->GetLeftNode()->GetId(), block_w->GetRightNode()->GetId()}));
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

void DefaultController::onFieldMousePress(const QMouseEvent *event) {
  auto &&_connection_map = _field_model.GetConnectionMap();
  auto &&_map_of_selected_nodes = _selection_model.GetSelectionMap();
  for (auto &&start_id : _connection_map.keys()) {
    QPoint start_pos, end_pos;

    auto &&start_pd = _field_model.GetBlockData(start_id.GetParentId());
    if (!start_pd) {
      assert(false);
      return;
    }

    auto &&start_data = _field_model.GetNodeData(start_id);
    if (!start_data) {
      assert(false);
      return;
    }
    NodeType start_type = start_data->node_type;

    start_pos = start_pd->pos + start_pd->offset[start_type];

    for (auto &&end_id : _connection_map[start_id]) {
      auto &&end_pd = _field_model.GetBlockData(end_id.GetParentId());
      if (!end_pd) {
        assert(false);
        return;
      }

      auto &&end_data = _field_model.GetNodeData(end_id);
      if (!end_data) {
        assert(false);
        return;
      }
      NodeType end_type = end_data->node_type;

      end_pos = end_pd->pos + end_pd->offset[end_type];
      bool find_line = isPointOnLine(QLine(start_pos, end_pos), event->pos());
      if (find_line) {
        _selection_model.AddSelection(start_id, end_id);
        return;
      }
    }
  }
  _selection_model.Clear();
}

void DefaultController::onFieldKeyPress(const QKeyEvent *event) {
  auto &&_connection_map = _field_model.GetConnectionMap();
  auto &&_map_of_selected_nodes = _selection_model.GetSelectionMap();
  if (event->key() == Qt::Key::Key_Delete) {
    _cm.Do(new RemoveCommand(_field_model, _selection_model, _active_nodes_model));
  }
}
