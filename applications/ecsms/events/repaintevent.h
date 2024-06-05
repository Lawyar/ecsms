#pragma once

#include "../models/linemodel.h"
#include "drawevent.h"

class RepaintEvent : public DrawEvent {
public:
  RepaintEvent() = default;
  virtual DrawEventType GetDrawEventType() const;
};
