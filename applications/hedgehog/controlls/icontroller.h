#pragma once

#include <QKeyEvent>
#include <QMouseEvent>

#include "../connectnodewidget.h"

class IController {
public:
  virtual void onMouseMoveEvent(QMouseEvent *event) = 0;

  virtual void onMousePressEvent(QMouseEvent *event) = 0;

  virtual void onKeyPressEvent(QKeyEvent *event) = 0;

  virtual void on_start(ConnectNodeWidget *node) = 0;
};
