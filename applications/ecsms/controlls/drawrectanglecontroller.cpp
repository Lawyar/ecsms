#include "drawrectanglecontroller.h"

DrawRectangleController::DrawRectangleController(
    PhantomRectangleModel &rect_model, SelectionModel &select_model,
    VisualizationModel &vis_model)
    : _rect_model(rect_model), _select_model(select_model),
      _vis_model(vis_model) {}

void DrawRectangleController::onMouseMoveEvent(QWidget *widget,
                                               QMouseEvent *event) {
  const QPoint vis_point = event->pos();
  const QPoint model_point = _vis_model.MapToModel(vis_point);

  if (auto &&field_w = qobject_cast<BlockFieldWidget *>(widget)) {
    _rect_model.SetP2(model_point);
  } else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _rect_model.SetP2(block_w->CoordToBlockField(model_point));
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    _rect_model.SetP2(connect_node_w->coordToBlockField(model_point));
  }
}

void DrawRectangleController::onMousePressEvent(QWidget *widget,
                                                QMouseEvent *event) {
  if (qobject_cast<BlockFieldWidget *>(widget)) {
    if (auto &&selected_node = _line_model.GetBegin()) {
      _line_model.SetBegin(std::nullopt, std::nullopt);
    }
  } else if (auto &&connect_node_w =
                 qobject_cast<ConnectNodeWidget *>(widget)) {
    auto &&connect_node_id = connect_node_w->GetId();
    onConnectNodeMousePress(connect_node_id);
  }
}

void DrawRectangleController::onKeyPressEvent(QWidget *widget,
                                              QKeyEvent *event) {}

void DrawRectangleController::onEnterEvent(QWidget *widget, QEvent *event) {}

void DrawRectangleController::onLeaveEvent(QWidget *widget, QEvent *event) {}

void DrawRectangleController::onMouseReleaseEvent(QWidget *widget,
                                                  QMouseEvent *event) {}
