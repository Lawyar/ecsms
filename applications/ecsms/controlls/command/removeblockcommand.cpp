#include "removecommand.h"
#include "../../blockwidget.h"

RemoveBlockCommand::RemoveBlockCommand(BlockId block,
                                       FieldModel::BlockData block_data,
                                       FieldModel &field_model,
                                       SelectionModel &selection_model,
                                       ActiveNodesModel &active_nodes_model)
    : _block(block), _block_data(block_data), _field_model(field_model),
      _selection_model(selection_model),
      _active_nodes_model(active_nodes_model) {}

void RemoveBlockCommand::Execute() {
  auto &&ln_id = _block.GetChildId(static_cast<PartId>(NodeType::Incoming));
  auto &&rn_id = _block.GetChildId(static_cast<PartId>(NodeType::Outgoing));

  // save connections
  _connections = _field_model.GetBlockConnections(_block);

  // save activities
  _activity[ln_id] = _active_nodes_model.GetNodeCount(ln_id);
  _activity[rn_id] = _active_nodes_model.GetNodeCount(rn_id);

  _selection_model.RemoveSelection(_block);
  _selection_model.RemoveSelectionWithNode(ln_id);
  _selection_model.RemoveSelectionWithNode(rn_id);

  _field_model.RemoveBlock(_block);
}

void RemoveBlockCommand::UnExecute() {

  _block_data.offset = {
      {NodeType::Incoming, BlockWidget::GetLeftNodeOffset(_block_data.text)},
      {NodeType::Outgoing, BlockWidget::GetRightNodeOffset(_block_data.text)},
  };
  QMap<NodeType, FieldModel::NodeData> node_data_map = {
      {NodeType::Incoming, {NodeType::Incoming}},
      {NodeType::Outgoing, {NodeType::Outgoing}}};
  _field_model.AddBlock(_block, _block_data, node_data_map);

  // connect all connections
  for (auto &&node : _connections.keys()) {
    for (auto &&start : _connections[node].keys()) {
      for (auto &&end : _connections[node][start]) {
        _field_model.AddConnection(start, end);
      }
    }
  }

  // restore activity
  auto &&ln_id = _block.GetChildId(static_cast<PartId>(NodeType::Incoming));
  auto &&rn_id = _block.GetChildId(static_cast<PartId>(NodeType::Outgoing));
  _active_nodes_model.IncreaseNodeCount(ln_id, _activity[ln_id]);
  _active_nodes_model.IncreaseNodeCount(rn_id, _activity[rn_id]);
}
