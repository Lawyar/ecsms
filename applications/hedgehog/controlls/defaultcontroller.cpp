#include "defaultcontroller.h"

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

void DefaultController::onMouseMoveEvent(QMouseEvent *event) {}

void DefaultController::onMousePressEvent(QMouseEvent *event) {
  auto &&_connection_map = _field_model.GetConnectionMap();
  auto &&_map_of_selected_nodes = _selection_model.GetSelectionMap();
  for (auto it = _connection_map.begin(); it != _connection_map.end(); ++it) {
    auto start = it.key();
    auto start_pos = start->coordToParent();
    for (auto end_node_it = it.value().begin(); end_node_it != it.value().end();
         ++end_node_it) {
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
}

void DefaultController::onKeyPressEvent(QKeyEvent *event) {
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

void DefaultController::on_start(ConnectNodeWidget *node) {
  _line_model.SetBegin(node);
}
