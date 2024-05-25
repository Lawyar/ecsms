#pragma once

enum EventType
{
	drawEvent, changeControllerEvent
};

class Event
{
public:
	virtual EventType GetEventType() const = 0;
};
