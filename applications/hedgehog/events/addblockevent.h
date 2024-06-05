#pragma once

#include "../models/fieldmodel.h"
#include "../namemaker/id.h"
#include "event.h"

#include <QPoint>

class AddBlockEvent : public Event {
public:
  AddBlockEvent(BlockId block, FieldModel::BlockData block_data);
  virtual EventType GetEventType() const override;
  BlockId GetId() const;
  FieldModel::BlockData GetBlockData() const;

private:
  BlockId _block;
  FieldModel::BlockData _block_data;
};