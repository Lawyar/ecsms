#pragma once

#include "icontroller.h"

class EmptyController : public IController {
 public:
  virtual void onMouseMoveEvent(QWidget* widget, QMouseEvent* event) override;
  virtual void onMousePressEvent(QWidget* widget, QMouseEvent* event) override;
  virtual void onKeyPressEvent(QWidget* widget, QKeyEvent* event) override;
  virtual void onEnterEvent(QWidget* widget, QEvent* event) override;
  virtual void onLeaveEvent(QWidget* widget, QEvent* event) override;
  virtual void onMouseReleaseEvent(QWidget* widget,
                                   QMouseEvent* event) override;
};
