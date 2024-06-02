#pragma once

#include "../connectnodewidget.h"
#include "../models/fieldmodel.h"
#include "../models/linemodel.h"
#include "activenodeslock.h"
#include "command/commandmanager.h"
#include "icontroller.h"

#include <memory>

class DrawLineController : public IController {
public:
  DrawLineController(FieldModel &field_model, LineModel &line_model,
                     const VisualizationModel &vis_model, CommandManager &cm);
  virtual void onMouseMoveEvent(QWidget *widget, QMouseEvent *event) override;
  virtual void onMousePressEvent(QWidget *widget, QMouseEvent *event) override;
  virtual void onKeyPressEvent(QWidget *widget, QKeyEvent *event) override;
  virtual void onEnterEvent(QWidget *widget, QEvent *event) override;
  virtual void onLeaveEvent(QWidget *widget, QEvent *event) override;
  virtual void onMouseReleaseEvent(QWidget *widget,
                                   QMouseEvent *event) override;

private:
  void onFieldMousePress();
  void onConnectNodeMousePress(NodeId node_id);

private:
  FieldModel &_field_model;
  LineModel &_line_model;
  const VisualizationModel &_vis_model;
  CommandManager &_cm;
  std::unique_ptr<ActiveNodesLock> _active_because_drawing,
      _active_because_entered;
};
