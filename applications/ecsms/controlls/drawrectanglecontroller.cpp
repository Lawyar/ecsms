#include "drawrectanglecontroller.h"
#include "../utility/selectionutility.h"
#include "../widgets/blockfieldwidget.h"

DrawRectangleController::DrawRectangleController(
    PhantomRectangleModel& rect_model,
    FieldModel& field_model,
    SelectionModel& select_model,
    VisualizationModel& vis_model)
    : _rect_model(rect_model),
      _field_model(field_model),
      _select_model(select_model),
      _vis_model(vis_model) {}

void DrawRectangleController::onMouseMoveEvent(QWidget* widget,
                                               QMouseEvent* event) {
  // upd select_model
  selectAllInRect(_field_model, _select_model, _vis_model, _rect_model);

  const QPoint model_point = _vis_model.MapToModel(event->pos());
  if (event->buttons() == Qt::LeftButton) {
    if (auto&& field_w = qobject_cast<BlockFieldWidget*>(widget)) {
      _rect_model.SetP1(model_point);
    } else if (auto&& block_w = qobject_cast<BlockWidget*>(widget)) {
      _rect_model.SetP1(block_w->CoordToBlockField(model_point));
    } else if (auto&& connect_node_w =
                   qobject_cast<ConnectNodeWidget*>(widget)) {
      _rect_model.SetP1(connect_node_w->coordToBlockField(model_point));
    }
  } else {
    _rect_model.SetP1(std::nullopt);
  }
}

void DrawRectangleController::onMousePressEvent(QWidget* widget,
                                                QMouseEvent* event) {}

void DrawRectangleController::onKeyPressEvent(QWidget* widget,
                                              QKeyEvent* event) {}

void DrawRectangleController::onEnterEvent(QWidget* widget, QEvent* event) {}

void DrawRectangleController::onLeaveEvent(QWidget* widget, QEvent* event) {}

void DrawRectangleController::onMouseReleaseEvent(QWidget* widget,
                                                  QMouseEvent* event) {
  _rect_model.SetP1(std::nullopt);
}
