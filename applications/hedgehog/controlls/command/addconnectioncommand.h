#pragma once

#include "../../models/fieldmodel.h"
#include "../../namemaker/id.h"
#include "icommand.h"

class AddConnectionCommand : public ICommand {
public:
  AddConnectionCommand(FieldModel &field_model, NodeId start, NodeId end);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  FieldModel &_field_model;
  FieldModel::Memento _field_model_save;
  NodeId _start, _end;
};
