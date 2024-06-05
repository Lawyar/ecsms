#include "removeblockevent.h"

RemoveBlockEvent::RemoveBlockEvent(BlockId block): _block(block) {}

EventType RemoveBlockEvent::GetEventType() const { return removeBlockEvent; }

BlockId RemoveBlockEvent::GetBlock() const { return _block; }
