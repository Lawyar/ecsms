#pragma once

#include "event.h"

enum ControllerType {
  defaultController,
  drawLineController,
  drawRectangleController,
  emptyCotroller
};

class ChangeControllerEvent : public Event {
 public:
  ChangeControllerEvent(ControllerType type);
  virtual EventType GetEventType() const override;
  virtual ControllerType GetControllerType();

 private:
  ControllerType _type;
};
