#pragma once

#include "../events/event.h"

#include <memory>

class IObserver
{
public:
  virtual ~IObserver() = default;
	virtual void Update(std::shared_ptr<Event> e) = 0;
};
