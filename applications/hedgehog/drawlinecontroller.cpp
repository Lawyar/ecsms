#include "controlls/drawlinecontroller.h"

DrawLineController::DrawLineController(
    std::shared_ptr<BlockField> model,
    std::shared_ptr<ConnectNodeWidget> selected_node)
    : _model(model), _start(selected_node) {}

void DrawLineController::onMouseMoveEvent(QMouseEvent *event) {
  if (!_selected_node)
    return;
  auto _pos = {event->x(), event->y()};
  // repaint();
}

void DrawLineController::onMousePressEvent(QMouseEvent *event) {
/* if (_selected_node) {
selected_node->makeTransparent(true);
selected_node = nullptr;
// repaint();
}*/
  if (_selected_node) // if any node was choosen or now it's the end of the
                      // connection
  {
    if (_selected_node != _start &&
        _selected_node->parent() != _start->parent()) {
      _model->GetConnectionMap[_selected_node].append(_start);
      _selected_node->makeTransparent(false);
      _start->makeTransparent(false);
    }
    _selected_node = nullptr;
    // repaint();
  } else // if none node was choosen or now it's the start of the connection
  {
    _selected_node = _start;
    _selected_node->makeTransparent(false);
    auto pos = _selected_node.get()->coordToParent();
    // repaint();
  }
}

void DrawLineController::onKeyPressEvent(QKeyEvent *event) {}
