#include "defaultcontroller.h"
#include "../blockfield.h"
#include "command/moveblockcommand.h"
#include "command/removecommand.h"

#include <QDebug>

static float distance(QPoint p1, QPoint p2) {
  float d = sqrt(pow((p1.x() - p2.x()), 2) + pow((p1.y() - p2.y()), 2));
  return d;
}

static bool isPointOnLine(QLine line, QPoint point) {
  float eps = 10;
  bool res = abs(distance(line.p1(), point) + distance(line.p2(), point) -
                 distance(line.p1(), line.p2())) < eps;
  return res;
}

DefaultController::DefaultController(FieldModel &field_model,
                                     SelectionModel &selection_model,
                                     LineModel &line_model,
                                     VisualizationModel &vis_model,
                                     CommandManager &cm)
    : _field_model(field_model), _selection_model(selection_model),
      _line_model(line_model), _vis_model(vis_model), _cm(cm) {}

void DefaultController::onMouseMoveEvent(QWidget *widget, QMouseEvent *event) {
  const QPoint vis_point = event->pos();

  if (qobject_cast<BlockField *>(widget)) {
    if (event->buttons() == Qt::LeftButton && _old_mouse_pos) {
      QPoint delta = vis_point - *_old_mouse_pos;
      _vis_model.SetNewCoordCenter(*_old_field_pos + delta);
    }
  }

  else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->buttons() == Qt::LeftButton && _old_mouse_pos) {
      QPoint delta = vis_point - *_old_mouse_pos;
      auto &&block_data = _field_model.GetBlockData(block_w->GetId());
      if (!block_data) {
        assert(false);
        return;
      }
      block_data->pos += delta;
      _field_model.SetBlockData(block_w->GetId(), *block_data);
    }
  }
}

void DefaultController::onMousePressEvent(QWidget *widget, QMouseEvent *event) {
  const QPoint vis_point = event->pos();

  if (qobject_cast<BlockField *>(widget)) {
    onFieldMousePress(event);
  }

  else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->button() == Qt::LeftButton) {
      _old_mouse_pos = vis_point;

      if (auto block_data = _field_model.GetBlockData(block_w->GetId()))
        _old_block_pos = block_data->pos;
      else {
        assert(false);
        _old_block_pos = std::nullopt;
      }
      _selection_model.AddSelection(block_w->GetId());
    }
  }

  else if (auto &&connect_node_w = qobject_cast<ConnectNodeWidget *>(widget)) {
    auto &&node_center = _vis_model.MapToModel(connect_node_w->getCenterCoordToBlockField());
    _line_model.SetBegin(connect_node_w->GetId(), node_center);
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
        _field_model,
        {block_w->GetLeftNode()->GetId(), block_w->GetRightNode()->GetId()},
        [this](const NodeId &node) {
          return _field_model.IsNodeConnected(node);
        }));
  }
}

void DefaultController::onLeaveEvent(QWidget *widget, QEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _active_nodes_lock.reset();
  }
}

void DefaultController::onMouseReleaseEvent(QWidget *widget,
                                            QMouseEvent *event) {
  if (qobject_cast<BlockField *>(widget)) {
    _old_field_pos = std::nullopt;
    _old_mouse_pos = std::nullopt;
  }

  else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (auto block_data = _field_model.GetBlockData(block_w->GetId());
        block_data && _old_block_pos && *_old_block_pos != block_data->pos) {
      _cm.Do(std::make_unique<MoveBlockCommand>(
          _field_model, block_w->GetId(), *_old_block_pos, block_data->pos));
    }
    _old_block_pos = std::nullopt;
    _old_mouse_pos = std::nullopt;
  }
}

void DefaultController::onFieldMousePress(const QMouseEvent *event) {
  const QPoint vis_point = event->pos();
  const QPoint model_point = _vis_model.MapToModel(vis_point);

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
      bool find_line = isPointOnLine(QLine(start_pos, end_pos), model_point);
      if (find_line) {
        _selection_model.AddSelection(start_id, end_id);
        return;
      }
    }
  }

  if (event->button() == Qt::LeftButton) {
    _old_field_pos = _vis_model.GetCenterCoord();
    _old_mouse_pos = vis_point;
  }

  _selection_model.Clear();
}

void DefaultController::onFieldKeyPress(const QKeyEvent *event) {
  auto &&_connection_map = _field_model.GetConnectionMap();
  auto &&_map_of_selected_nodes = _selection_model.GetSelectionMap();
  if (event->key() == Qt::Key::Key_Delete) {
    auto selected_connections = _selection_model.GetSelectionMap();
    auto selected_blocks = _selection_model.GetSelectedBlocks();
    if (_active_nodes_lock) {
      auto &&lock_nodes = _active_nodes_lock->GetLockedNodes();
      for (auto &&block : selected_blocks) {
        for (auto &&lock_node : lock_nodes) {
          if (lock_node.GetParentId() == block) {
            _active_nodes_lock.reset();
            break;
          }
        }
      }
    }
    _selection_model.Clear();
    _cm.Do(std::make_unique<RemoveCommand>(_field_model, selected_connections,
                                           selected_blocks));
  }
}
