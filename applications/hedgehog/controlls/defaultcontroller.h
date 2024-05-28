#pragma once

#include "icontroller.h"
#include "../models/activenodesmodel.h"
#include "../models/fieldmodel.h"
#include "../models/linemodel.h"
#include "../models/selectionmodel.h"
#include "activenodeslock.h"
#include "command/commandmanager.h"

#include <optional>

class DefaultController : public IController {
public:
  DefaultController(FieldModel &field_model, SelectionModel &selection_model,
                    LineModel &line_model, ActiveNodesModel &active_nodes, CommandManager &cm);
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
  ActiveNodesModel &_active_nodes_model;
  CommandManager &_cm;
  std::optional<QPoint> _old_block_pos;
  std::unique_ptr<ActiveNodesLock> _active_nodes_lock;
};
