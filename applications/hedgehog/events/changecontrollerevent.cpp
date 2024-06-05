#include "changecontrollerevent.h"

ChangeControllerEvent::ChangeControllerEvent(ControllerType type)
    : _type(type) {}

EventType ChangeControllerEvent::GetEventType() const {
  return changeControllerEvent;
}

ControllerType ChangeControllerEvent::GetControllerType() { return _type; }
