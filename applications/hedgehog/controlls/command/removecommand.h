#pragma once

#include "../../models/fieldmodel.h"
#include "../../namemaker/id.h"
#include "icommand.h"

#include <QLabel>
#include <QPoint>
#include <set>

class RemoveCommand : public ICommand {
public:
  RemoveCommand(FieldModel &field_model,
                const QMap<NodeId, std::vector<NodeId>> &selected_connections,
                const std::set<BlockId> &selected_blocks);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  FieldModel &_field_model;
  const FieldModel::Memento _field_model_save;
  const QMap<NodeId, std::vector<NodeId>> _selected_connections;
  const std::set<BlockId> _selected_blocks;
};
