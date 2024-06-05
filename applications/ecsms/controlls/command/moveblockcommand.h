#pragma once

#include "icommand.h"
#include "../../namemaker/id.h"
#include "../../models/fieldmodel.h"

class MoveBlockCommand : public ICommand {
public:
  MoveBlockCommand(FieldModel &field_model, BlockId block, QPoint old_pos,
                   QPoint new_pos);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  FieldModel &_field_model;
  BlockId _block;
  QPoint _old_pos, _new_pos;
};
