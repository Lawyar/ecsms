#include "removecommand.h"
#include "../../blockwidget.h"

RemoveCommand::RemoveCommand(FieldModel &field_model,
                             SelectionModel &selection_model/*,
                             ActiveNodesModel &active_nodes_model*/)
    : _field_model(field_model), _field_model_save(field_model.Save()),
      _selection_model(selection_model),
      _selection_model_save(selection_model.Save())/*,
      _active_nodes_model(active_nodes_model),
      _active_nodes_model_save(active_nodes_model.Save())*/ {}

void RemoveCommand::Execute() {
  auto removeConnection = [this](const QMap<NodeId, std::vector<NodeId>> &map) {
    for (auto &&start : map.keys()) {
      for (auto &&end : map[start]) {
        //_active_nodes_model.DecreaseNodeCount(start);
        //_active_nodes_model.DecreaseNodeCount(end);
        _field_model.RemoveConnection(start, end);
      }
    }
  };

  removeConnection(_selection_model.GetSelectionMap());
  

  auto selected_blocks = _selection_model.GetSelectedBlocks();
  _selection_model.Clear();
  for (auto &&block : selected_blocks) {
    auto &&ln_id = block.GetChildId(static_cast<PartId>(NodeType::Incoming));
    auto &&rn_id = block.GetChildId(static_cast<PartId>(NodeType::Outgoing));

    removeConnection(_field_model.GetNodeConnections(ln_id));
    removeConnection(_field_model.GetNodeConnections(rn_id));

    _field_model.RemoveBlock(block);
  }
}

void RemoveCommand::UnExecute() {
  _field_model.Load(_field_model_save);
  _selection_model.Load(_selection_model_save);
  //_active_nodes_model.Load(_active_nodes_model_save);
}
