#include "addconnectioncommand.h"

AddConnectionCommand::AddConnectionCommand(FieldModel &field_model, NodeId start,
                                           NodeId end)
    : _field_model(field_model), _field_model_save(field_model.Save()),
      _start(start), _end(end) {}

void AddConnectionCommand::Execute() {
  _field_model.AddConnection(_start, _end);
}

void AddConnectionCommand::UnExecute() { _field_model.Load(_field_model_save); }
