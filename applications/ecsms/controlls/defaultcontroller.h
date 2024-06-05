#pragma once

#include "../models/fieldmodel.h"
#include "../models/linemodel.h"
#include "../models/selectionmodel.h"
#include "../models/visualizationmodel.h"
#include "activenodeslock.h"
#include "command/commandmanager.h"
#include "icontroller.h"

#include <optional>

class DefaultController : public IController {
public:
  DefaultController(FieldModel &field_model, SelectionModel &selection_model,
                    LineModel &line_model, VisualizationModel &vis_model,
                    CommandManager &cm);
  virtual void onMouseMoveEvent(QWidget *widget, QMouseEvent *event) override;
  virtual void onMousePressEvent(QWidget *widget, QMouseEvent *event) override;
  virtual void onKeyPressEvent(QWidget *widget, QKeyEvent *event) override;
  virtual void onEnterEvent(QWidget *widget, QEvent *event) override;
  virtual void onLeaveEvent(QWidget *widget, QEvent *event) override;
  virtual void onMouseReleaseEvent(QWidget *widget,
                                   QMouseEvent *event) override;

private:
  void onFieldMousePress(const QMouseEvent *event);
  void onFieldKeyPress(const QKeyEvent *event);

private:
  FieldModel &_field_model;
  SelectionModel &_selection_model;
  LineModel &_line_model;
  VisualizationModel &_vis_model;
  CommandManager &_cm;
  std::optional<QPoint> _old_mouse_pos, _old_block_pos, _old_field_pos;
  std::unique_ptr<ActiveNodesLock> _active_nodes_lock;
};
