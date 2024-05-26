#pragma once

#include "../connectnodewidget.h"

#include <QKeyEvent>
#include <QMouseEvent>

class IController {
public:
  virtual ~IController() = default;
  virtual void onMouseMoveEvent(QWidget *widget, QMouseEvent *event) = 0;
  virtual void onMousePressEvent(QWidget *widget, QMouseEvent *event) = 0;
  virtual void onKeyPressEvent(QWidget *widget, QKeyEvent *event) = 0;
  virtual void onEnterEvent(QWidget *widget, QEvent *event) = 0;
  virtual void onLeaveEvent(QWidget *widget, QEvent *event) = 0;
  virtual void onMouseReleaseEvent(QWidget *widget, QMouseEvent *event) = 0;
};
