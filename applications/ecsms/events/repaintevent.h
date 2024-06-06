#pragma once

#include "../models/phantomlinemodel.h"
#include "drawevent.h"

class RepaintEvent : public DrawEvent {
public:
  RepaintEvent() = default;
  virtual DrawEventType GetDrawEventType() const;
};
