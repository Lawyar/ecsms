#include "drawlinecontroller.h"
#include "../blockfield.h"

DrawLineController::DrawLineController(FieldModel &model, LineModel &line_model,
                                       ActiveNodesModel &active_nodes_model)
    : _field_model(model), _line_model(line_model),
      _active_nodes_model(active_nodes_model) {
  auto &&node_id = active_nodes_model.GetBeginOfLine();
  assert(node_id);
  _active_because_drawing.reset(
      new ActiveNodesLock(_active_nodes_model, {*node_id}));
  _active_because_entered.reset(
      new ActiveNodesLock(_active_nodes_model, {*node_id}));
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
  if (qobject_cast<BlockField *>(widget)) {
    onFieldMousePress();
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    auto &&connect_node_id = connect_node_w->GetId();
    onConnectNodeMousePress(connect_node_id);
  }
}

void DrawLineController::onKeyPressEvent(QWidget *widget, QKeyEvent *event) {}

void DrawLineController::onEnterEvent(QWidget *widget, QEvent *event) {
  if (auto &&connect_node_w = qobject_cast<ConnectNodeWidget *>(widget)) {
    _active_because_entered.reset(
        new ActiveNodesLock(_active_nodes_model, {connect_node_w->GetId()}));
  }
}

void DrawLineController::onLeaveEvent(QWidget *widget, QEvent *event) {
  if (auto &&connect_node_w = qobject_cast<ConnectNodeWidget *>(widget)) {
    _active_because_entered.reset();
  }
}

void DrawLineController::onMouseReleaseEvent(QWidget *widget,
                                             QMouseEvent *event) {}

void DrawLineController::onFieldMousePress() {
  if (auto &&selected_node = _line_model.GetBegin()) {
    _line_model.SetBegin(std::nullopt);
  }
}

void DrawLineController::onConnectNodeMousePress(NodeId connect_node_id) {
  auto start_id = _active_nodes_model.GetBeginOfLine();
  if (!start_id) {
    assert(false);
    return;
  }

  if (connect_node_id != *start_id &&
      connect_node_id.GetParentId() != start_id->GetParentId()) {
    _field_model.AddConnection(*start_id, connect_node_id);
    _active_nodes_model.IncreaseNodeCount(*start_id);
    _active_nodes_model.IncreaseNodeCount(connect_node_id);
  }
  _line_model.SetBegin(std::nullopt);
}
