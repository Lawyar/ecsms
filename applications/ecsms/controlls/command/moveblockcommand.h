#pragma once

#include "icommand.h"
#include "../../namemaker/id.h"
#include "../../models/fieldmodel.h"

class MoveBlockCommand : public ICommand {
public:
  MoveBlockCommand(FieldModel &field_model, BlockId block, QPoint delta);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  FieldModel &_field_model;
  BlockId _block;
  QPoint _delta;
};
