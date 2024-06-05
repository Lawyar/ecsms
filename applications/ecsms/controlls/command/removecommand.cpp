#include "removecommand.h"
#include "../../widgets/blockwidget.h"

RemoveCommand::RemoveCommand(
    FieldModel &field_model,
    const QMap<NodeId, std::vector<NodeId>> &selected_connections,
    const std::set<BlockId> &selected_blocks)
    : _field_model(field_model), _field_model_save(field_model.Save()),
      _selected_connections(selected_connections),
      _selected_blocks(selected_blocks) {}

void RemoveCommand::Execute() {
  auto removeConnection = [this](const QMap<NodeId, std::vector<NodeId>> &map) {
    for (auto &&start : map.keys()) {
      for (auto &&end : map[start]) {
        _field_model.RemoveConnection(start, end);
      }
    }
  };

  removeConnection(_selected_connections);

  for (auto &&block : _selected_blocks) {
    auto &&ln_id = block.GetChildId(static_cast<PartId>(NodeType::Incoming));
    auto &&rn_id = block.GetChildId(static_cast<PartId>(NodeType::Outgoing));

    removeConnection(_field_model.GetNodeConnections(ln_id));
    removeConnection(_field_model.GetNodeConnections(rn_id));

    _field_model.RemoveBlock(block);
  }
}

void RemoveCommand::UnExecute() { _field_model.Load(_field_model_save); }
