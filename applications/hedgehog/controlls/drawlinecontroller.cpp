#include "drawlinecontroller.h"

DrawLineController::DrawLineController(FieldModel &model, LineModel &line_model)
    : _field_model(model), _line_model(line_model) {}

void DrawLineController::onMouseMoveEvent(QMouseEvent *event) {
  _line_model.SetEnd(event->pos());
}

void DrawLineController::onMousePressEvent(QMouseEvent *event) {
  if (auto &&selected_node = _line_model.GetBegin()) {
    selected_node->makeTransparent(true);
    _line_model.SetBegin(nullptr);
  }
}

void DrawLineController::onKeyPressEvent(QKeyEvent *event) {}

void DrawLineController::on_start(ConnectNodeWidget *selected_node) {
  if (selected_node) {
    auto start = _line_model.GetBegin();
    if (selected_node != start && selected_node->parent() != start->parent()) {
      _field_model.AddConnection(start, selected_node);
      selected_node->makeTransparent(false);
      start->makeTransparent(false);
    }
    _line_model.SetBegin(nullptr);
  } else {
    assert(false);
  }
}
