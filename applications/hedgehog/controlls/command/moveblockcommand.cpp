#include "moveblockcommand.h"

MoveBlockCommand::MoveBlockCommand(FieldModel &field_model,
                                   VisualizationModel &vis_model, BlockId block,
                                   QPoint old_pos, QPoint new_pos)
    : _field_model(field_model), _vis_model(vis_model), _block(block),
      _old_pos(old_pos), _new_pos(new_pos) {}

void MoveBlockCommand::Execute() {
  auto &&block_data = _field_model.GetBlockData(_block);
  if (!block_data) {
    assert(false);
    return;
  }
  FieldModel::BlockData new_block_data = {_vis_model.MapToVisualization(_new_pos),
                                          block_data->offset,
                                          block_data->text};
  _field_model.SetBlockData(_block, new_block_data);
}

void MoveBlockCommand::UnExecute() {
  auto &&new_block_data = _field_model.GetBlockData(_block);
  if (!new_block_data) {
    assert(false);
    return;
  }
  new_block_data->pos = _vis_model.MapToVisualization(_old_pos);
  _field_model.SetBlockData(_block, *new_block_data);
}
