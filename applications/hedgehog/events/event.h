#pragma once

enum EventType
{
	drawEvent, changeControllerEvent, changeActiveNodeEvent, addBlockEvent, removeBlockEvent
};

class Event
{
public:
  virtual ~Event() = default;
	virtual EventType GetEventType() const = 0;
};
