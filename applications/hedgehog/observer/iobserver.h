#pragma once

#include "../events/event.h"

class IObserver
{
public:
	virtual void Update(std::shared_ptr<Event> e) = 0;
};
