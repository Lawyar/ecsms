#pragma once

#include "../models/fieldmodel.h"
#include "../models/linemodel.h"
#include "../models/selectionmodel.h"
#include "icontroller.h"

class DefaultController : public IController {
public:
  DefaultController(FieldModel &field_model, SelectionModel &selection_model,
                    LineModel &line_model);

  virtual void onMouseMoveEvent(QMouseEvent *event) override;

  virtual void onMousePressEvent(QMouseEvent *event) override;

  virtual void onKeyPressEvent(QKeyEvent *event) override;

  virtual void on_start(ConnectNodeWidget *node) override;

private:
  FieldModel &_field_model;
  SelectionModel &_selection_model;
  LineModel &_line_model;
};
