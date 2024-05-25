#pragma once

#include "../connectnodewidget.h"
#include "../models/fieldmodel.h"
#include "../models/linemodel.h"
#include "icontroller.h"

#include <memory>

class DrawLineController : public IController {
public:
  DrawLineController(FieldModel &model, LineModel &line_model);

  virtual void onMouseMoveEvent(QMouseEvent *event) override;

  virtual void onMousePressEvent(QMouseEvent *event) override;

  virtual void onKeyPressEvent(QKeyEvent *event) override;

  virtual void on_start(ConnectNodeWidget *selected_node) override;

private:
  FieldModel &_field_model;
  LineModel &_line_model;
};
