#include "addblockevent.h"

AddBlockEvent::AddBlockEvent(BlockId block, FieldModel::BlockData block_data)
    : _block(block), _block_data(block_data) {}

EventType AddBlockEvent::GetEventType() const { return addBlockEvent; }

BlockId AddBlockEvent::GetId() const { return _block; }

FieldModel::BlockData AddBlockEvent::GetBlockData() const {
  return _block_data;
}
