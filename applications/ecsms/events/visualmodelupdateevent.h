#pragma once

#include "../models/phantomlinemodel.h"
#include "drawevent.h"

class VisualModelUpdateEvent : public Event {
public:
  VisualModelUpdateEvent() = default;
  virtual EventType GetEventType() const;
};
