#pragma once

#include "../namemaker/id.h"
#include "event.h"
#include "../models/fieldmodel.h"

class UpdateBlockEvent : public Event {
public:
  UpdateBlockEvent(BlockId block, FieldModel::BlockData bd);
  virtual EventType GetEventType() const override;
  BlockId GetBlock() const; 
  FieldModel::BlockData GetBlockData() const;

private:
  BlockId _block;
  FieldModel::BlockData _bd;
};
