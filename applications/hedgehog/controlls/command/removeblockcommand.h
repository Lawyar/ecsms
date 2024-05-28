#pragma once

#include "../../models/fieldmodel.h"
#include "../../models/selectionmodel.h"
#include "../../models/activenodesmodel.h"
#include "../../namemaker/id.h"
#include "icommand.h"

#include <QLabel>
#include <QPoint>

class RemoveBlockCommand : public ICommand {
public:
  RemoveBlockCommand(BlockId block, FieldModel::BlockData block_data,
                  FieldModel &field_model, SelectionModel &selection_model,
                  ActiveNodesModel &active_nodes_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  BlockId _block;
  FieldModel::BlockData _block_data;
  std::map<NodeId, int> _activity;
  FieldModel &_field_model;
  SelectionModel &_selection_model;
  ActiveNodesModel &_active_nodes_model;
  QMap<NodeId, QMap<NodeId, std::vector<NodeId>>> _connections;
};
