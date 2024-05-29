#pragma once

#include "../../models/fieldmodel.h"
#include "../../models/selectionmodel.h"
#include "../../models/activenodesmodel.h"
#include "../../namemaker/id.h"
#include "icommand.h"

#include <QLabel>
#include <QPoint>

class RemoveCommand : public ICommand {
public:
  RemoveCommand(FieldModel &field_model, SelectionModel &selection_model/*,
                  ActiveNodesModel &active_nodes_model*/);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  FieldModel &_field_model;
  FieldModel::Memento _field_model_save;
  SelectionModel &_selection_model;
  SelectionModel::Memento _selection_model_save;
  // ActiveNodesModel &_active_nodes_model;
  // ActiveNodesModel::Memento _active_nodes_model_save;
  QMap<NodeId, QMap<NodeId, std::vector<NodeId>>> _connections;
};
