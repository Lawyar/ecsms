#pragma once

#include "../models/linemodel.h"
#include "drawevent.h"

class DrawLineEvent : public DrawEvent {
public:
  DrawLineEvent(QPoint begin, QPoint end);
  virtual DrawEventType GetDrawEventType() const;
  const QLine GetLine() const;

private:
  QPoint _begin, _end;
};
