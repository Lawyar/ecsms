#pragma once

#include "event.h"

enum DrawEventType
{
  repaintEvent,
};

class DrawEvent: public Event
{
public:
	virtual DrawEventType GetDrawEventType() const = 0;
	virtual EventType GetEventType() const override;
};
