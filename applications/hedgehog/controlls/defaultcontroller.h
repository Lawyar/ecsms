#pragma once

#include "../models/fieldmodel.h"
#include "../models/linemodel.h"
#include "../models/selectionmodel.h"
#include "icontroller.h"

#include <optional>

class DefaultController : public IController {
public:
  DefaultController(FieldModel &field_model, SelectionModel &selection_model,
                    LineModel &line_model);
  virtual void onMouseMoveEvent(QWidget *widget, QMouseEvent *event) override;
  virtual void onMousePressEvent(QWidget *widget, QMouseEvent *event) override;
  virtual void onKeyPressEvent(QWidget *widget, QKeyEvent *event) override;
  virtual void onLeaveEvent(QWidget *widget, QEvent *event) override;
  virtual void onMouseReleaseEvent(QWidget *widget,
                                   QMouseEvent *event) override;

private:
  FieldModel &_field_model;
  SelectionModel &_selection_model;
  LineModel &_line_model;
  std::optional<QPoint> _old_block_pos;
};
