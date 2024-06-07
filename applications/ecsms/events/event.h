#pragma once

enum EventType {
  drawEvent,
  changeControllerEvent,
  changeActiveNodeEvent,
  addBlockEvent,
  updateBlockEvent,
  removeBlockEvent,
  visualModelUpdateEvent
};

class Event {
public:
  virtual ~Event() = default;
  virtual EventType GetEventType() const = 0;
};
