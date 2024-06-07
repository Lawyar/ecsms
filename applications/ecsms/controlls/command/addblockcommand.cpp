#include "addblockcommand.h"
#include "../../widgets/blockwidget.h"

AddBlockCommand::AddBlockCommand(BlockId block, QPoint pos,
                                 const QString &text,
                                 FieldModel &field_model,
                                 SelectionModel &selection_model)
    : _block(block), _field_model(field_model),
      _selection_model(selection_model) {
  _block_data.pos = pos;
  _block_data.size = BlockWidget::GetSize(text);
  _block_data.text = text;
}

void AddBlockCommand::Execute() {
  _block_data.offset = {
      {NodeType::Incoming, BlockWidget::GetLeftNodeOffset(_block_data.text)},
      {NodeType::Outgoing, BlockWidget::GetRightNodeOffset(_block_data.text)},
  };
  QMap<NodeType, FieldModel::NodeData> node_data_map = {
      {NodeType::Incoming, {NodeType::Incoming}},
      {NodeType::Outgoing, {NodeType::Outgoing}}};
  _field_model.AddBlock(_block, _block_data, node_data_map);
}

void AddBlockCommand::UnExecute() {
  auto &&ln_id = _block.GetChildId(static_cast<PartId>(NodeType::Incoming));
  auto &&rn_id = _block.GetChildId(static_cast<PartId>(NodeType::Outgoing));
  
  _selection_model.RemoveSelection(_block);
  _selection_model.RemoveSelectionWithNode(ln_id);
  _selection_model.RemoveSelectionWithNode(rn_id);

  _field_model.RemoveBlock(_block);
}
