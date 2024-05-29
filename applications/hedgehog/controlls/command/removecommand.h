#pragma once

#include "../../models/fieldmodel.h"
#include "../../models/selectionmodel.h"
#include "../../namemaker/id.h"
#include "icommand.h"

#include <QLabel>
#include <QPoint>

class RemoveCommand : public ICommand {
public:
  RemoveCommand(FieldModel &field_model, SelectionModel &selection_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  FieldModel &_field_model;
  FieldModel::Memento _field_model_save;
  SelectionModel &_selection_model;
  SelectionModel::Memento _selection_model_save;
  QMap<NodeId, QMap<NodeId, std::vector<NodeId>>> _connections;
};
