#include "drawlinecontroller.h"
#include "../blockfield.h"

DrawLineController::DrawLineController(FieldModel &model, LineModel &line_model,
                                       ActiveNodesModel &active_nodes_model)
    : _field_model(model), _line_model(line_model),
      _active_nodes_model(active_nodes_model) {
  auto &&node = _line_model.GetBegin();
  _active_because_drawing.reset(
      new ActiveNodesLock(_active_nodes_model, {node}));
  _active_because_entered.reset(
      new ActiveNodesLock(_active_nodes_model, {node}));
}

void DrawLineController::onMouseMoveEvent(QWidget *widget, QMouseEvent *event) {
  if (auto &&field_w = qobject_cast<BlockField *>(widget)) {
    _line_model.SetEnd(event->pos());
  } else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _line_model.SetEnd(block_w->coordToBlockField(event->pos()));
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    _line_model.SetEnd(connect_node_w->coordToBlockField(event->pos()));
  }
}

void DrawLineController::onMousePressEvent(QWidget *widget,
                                           QMouseEvent *event) {
  if (auto &&field_w = qobject_cast<BlockField *>(widget)) {
    if (auto &&selected_node = _line_model.GetBegin()) {
      _line_model.SetBegin(nullptr);
    }
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    if (connect_node_w) {
      auto start = _line_model.GetBegin();
      if (connect_node_w != start &&
          connect_node_w->parent() != start->parent()) {
        _field_model.AddConnection(start->GetId(), connect_node_w->GetId());
        _active_nodes_model.IncreaseNodeCount(start->GetId());
        _active_nodes_model.IncreaseNodeCount(connect_node_w->GetId());
      }
      _line_model.SetBegin(nullptr);
    } else {
      assert(false);
    }
  }
}

void DrawLineController::onKeyPressEvent(QWidget *widget, QKeyEvent *event) {}

void DrawLineController::onEnterEvent(QWidget *widget, QEvent *event) {
  if (auto &&connect_node_w = qobject_cast<ConnectNodeWidget *>(widget)) {
    _active_because_entered.reset(
        new ActiveNodesLock(_active_nodes_model, {connect_node_w}));
  }
}

void DrawLineController::onLeaveEvent(QWidget *widget, QEvent *event) {
  if (auto &&connect_node_w = qobject_cast<ConnectNodeWidget *>(widget)) {
    _active_because_entered.reset();
  }
}

void DrawLineController::onMouseReleaseEvent(QWidget *widget,
                                             QMouseEvent *event) {}
