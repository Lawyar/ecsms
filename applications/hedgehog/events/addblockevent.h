#pragma once

#include "../namemaker/id.h"
#include "event.h"

#include <QPoint>

class AddBlockEvent : public Event {
public:
  AddBlockEvent(Id block, QPoint pos);
  virtual EventType GetEventType() const override;
  Id GetId() const;
  QPoint GetPos() const;

private:
  Id _block;
  QPoint _pos;
};