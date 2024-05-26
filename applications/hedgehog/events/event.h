#pragma once

enum EventType
{
	drawEvent, changeControllerEvent, changeActiveNodeEvent
};

class Event
{
public:
  virtual ~Event() = default;
	virtual EventType GetEventType() const = 0;
};
