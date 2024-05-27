#pragma once

#include "../../connectnodewidget.h"
#include "../../models/fieldmodel.h"
#include "icommand.h"

#include <map>
#include <set>

class DeleteBlocksAndConnectionsCommand : public ICommand {
public:
  DeleteBlocksAndConnectionsCommand(
      const QMap<NodeId, std::vector<NodeId>> &connection_map,
      const std::set<BlockId> &blocks, FieldModel &field_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  void saveData();

private:
  QMap<NodeId, std::vector<NodeId>> _connection_map;
  std::set<BlockId> _blocks;
  FieldModel &_field_model;

  std::map<int, BlockId> blocks_ids;
};
