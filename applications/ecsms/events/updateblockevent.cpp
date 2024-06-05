#include "updateblockevent.h"

UpdateBlockEvent::UpdateBlockEvent(BlockId block, FieldModel::BlockData bd)
    : _block(block), _bd(bd) {}

EventType UpdateBlockEvent::GetEventType() const { return updateBlockEvent; }

BlockId UpdateBlockEvent::GetBlock() const { return _block; }

FieldModel::BlockData UpdateBlockEvent::GetBlockData() const {
  return _bd;
}
