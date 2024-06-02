#include "drawlinecontroller.h"
#include "../blockfield.h"
#include "command/addconnectioncommand.h"

DrawLineController::DrawLineController(FieldModel &field_model,
                                       LineModel &line_model,
                                       const VisualizationModel &vis_model,
                                       CommandManager &cm)
    : _field_model(field_model), _line_model(line_model), _vis_model(vis_model),
      _cm(cm) {
  auto &&node_id = _line_model.GetBeginNode();
  assert(node_id);
  _active_because_drawing.reset(new ActiveNodesLock(
      _field_model, {*node_id}, [this](const NodeId &node) -> bool {
        return _field_model.IsNodeConnected(node) ||
               _line_model.GetBeginNode() == node;
      }));
  _active_because_entered.reset(
      new ActiveNodesLock(_field_model, {*node_id}, [this](const NodeId &node) {
        return _field_model.IsNodeConnected(node) ||
               _line_model.GetBeginNode() == node;
      }));
}

void DrawLineController::onMouseMoveEvent(QWidget *widget, QMouseEvent *event) {
  const QPoint vis_point = event->pos();
  const QPoint model_point = _vis_model.MapToModel(vis_point);

  if (auto &&field_w = qobject_cast<BlockField *>(widget)) {
    _line_model.SetEnd(model_point);
  } else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _line_model.SetEnd(block_w->CoordToBlockField(model_point));
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    _line_model.SetEnd(connect_node_w->coordToBlockField(model_point));
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
    _active_because_entered.reset(new ActiveNodesLock(
        _field_model, {connect_node_w->GetId()}, [this](const NodeId &node) {
          return _field_model.IsNodeConnected(node);
        }));
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
    _line_model.SetBegin(std::nullopt, std::nullopt);
  }
}

void DrawLineController::onConnectNodeMousePress(NodeId connect_node_id) {
  auto start_id = _line_model.GetBeginNode();
  if (!start_id) {
    assert(false);
    return;
  }

  if (connect_node_id != *start_id &&
      connect_node_id.GetParentId() != start_id->GetParentId()) {
    _cm.Do(std::make_unique<AddConnectionCommand>(_field_model, *start_id,
                                                  connect_node_id));
  }

  _line_model.SetBegin(std::nullopt, std::nullopt);
}
