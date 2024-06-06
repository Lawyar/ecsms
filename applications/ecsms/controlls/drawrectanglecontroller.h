#pragma once

#include "icontroller.h"
#include "../models/phantomrectanglemodel.h"
#include "../models/selectionmodel.h"
#include "../models/visualizationmodel.h"

class DrawRectangleController : public IController {
public:
  DrawRectangleController(PhantomRectangleModel &rect_model,
                          SelectionModel &select_model,
                          VisualizationModel &vis_model);
  virtual void onMouseMoveEvent(QWidget *widget, QMouseEvent *event) override;
  virtual void onMousePressEvent(QWidget *widget, QMouseEvent *event) override;
  virtual void onKeyPressEvent(QWidget *widget, QKeyEvent *event) override;
  virtual void onEnterEvent(QWidget *widget, QEvent *event) override;
  virtual void onLeaveEvent(QWidget *widget, QEvent *event) override;
  virtual void onMouseReleaseEvent(QWidget *widget,
                                   QMouseEvent *event) override;

private:
  PhantomRectangleModel &_rect_model;
  SelectionModel &_select_model;
  VisualizationModel &_vis_model;
};
