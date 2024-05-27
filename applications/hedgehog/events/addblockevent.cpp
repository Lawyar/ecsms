#include "addblockevent.h"

AddBlockEvent::AddBlockEvent(Id block, QPoint pos) : _block(block), _pos(pos) {}

EventType AddBlockEvent::GetEventType() const { return addBlockEvent; }

Id AddBlockEvent::GetId() const { return _block; }

QPoint AddBlockEvent::GetPos() const { return _pos; }
