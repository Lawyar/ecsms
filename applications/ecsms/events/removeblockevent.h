#pragma once

#include "event.h"
#include "../namemaker/id.h"

class RemoveBlockEvent : public Event {
public:
  RemoveBlockEvent(BlockId block);
  virtual EventType GetEventType() const override;
  BlockId GetBlock() const;

private:
  BlockId _block;
};
