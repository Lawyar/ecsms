#include "moveblockcommand.h"

MoveBlockCommand::MoveBlockCommand(FieldModel &field_model, BlockId block,
                                   QPoint delta)
    : _field_model(field_model), _block(block), _delta(delta) {}

void MoveBlockCommand::Execute() {
  auto &&block_data = _field_model.GetBlockData(_block);
  if (!block_data) {
    assert(false);
    return;
  }

  block_data->pos += _delta;
  _field_model.SetBlockData(_block, *block_data);
}

void MoveBlockCommand::UnExecute() {
  auto &&block_data = _field_model.GetBlockData(_block);
  if (!block_data) {
    assert(false);
    return;
  }

  block_data->pos -= _delta;
  _field_model.SetBlockData(_block, *block_data);
}
