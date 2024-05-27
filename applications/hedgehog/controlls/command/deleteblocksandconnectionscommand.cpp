#include "deleteblocksandconnectionscommand.h"

DeleteBlocksAndConnectionsCommand::DeleteBlocksAndConnectionsCommand(
    const QMap<NodeId, std::vector<NodeId>> &connection_map,
    const std::set<BlockId> &blocks, FieldModel &field_model)
    : _connection_map(connection_map), _blocks(blocks),
      _field_model(field_model) {}

void DeleteBlocksAndConnectionsCommand::Execute() {}

void DeleteBlocksAndConnectionsCommand::UnExecute() {}

void DeleteBlocksAndConnectionsCommand::saveData() {
  int count = 0;
  for (auto &&block : _blocks) {
    blocks_ids.emplace(count, block);
    ++count;
  }
}
