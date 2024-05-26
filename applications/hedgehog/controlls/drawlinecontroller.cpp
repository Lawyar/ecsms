#include "drawlinecontroller.h"
#include "../blockfield.h"

DrawLineController::DrawLineController(FieldModel &model, LineModel &line_model)
    : _field_model(model), _line_model(line_model) {}

void DrawLineController::onMouseMoveEvent(QWidget *widget, QMouseEvent *event) {
  if (auto &&field_w = qobject_cast<BlockField *>(widget)) {
    _line_model.SetEnd(event->pos());
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    connect_node_w->makeTransparent(false);
  }
}

void DrawLineController::onMousePressEvent(QWidget *widget,
                                           QMouseEvent *event) {
  if (auto &&field_w = qobject_cast<BlockField *>(widget)) {
    if (auto &&selected_node = _line_model.GetBegin()) {
      selected_node->makeTransparent(true);
      _line_model.SetBegin(nullptr);
    }
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    if (connect_node_w) {
      auto start = _line_model.GetBegin();
      if (connect_node_w != start &&
          connect_node_w->parent() != start->parent()) {
        _field_model.AddConnection(start, connect_node_w);
        connect_node_w->makeTransparent(false);
        start->makeTransparent(false);
      }
      _line_model.SetBegin(nullptr);
    } else {
      assert(false);
    }
  }
}

void DrawLineController::onKeyPressEvent(QWidget *widget, QKeyEvent *event) {}

void DrawLineController::onLeaveEvent(QWidget *widget, QEvent *event) {
  if (auto &&connect_node_w = qobject_cast<ConnectNodeWidget *>(widget)) {
    connect_node_w->makeTransparent(false);
  }
}

void DrawLineController::onMouseReleaseEvent(QWidget *widget,
                                             QMouseEvent *event) {}
