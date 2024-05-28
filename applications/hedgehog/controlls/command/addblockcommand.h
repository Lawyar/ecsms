#pragma once

#include "../../models/fieldmodel.h"
#include "../../models/selectionmodel.h"
#include "../../namemaker/id.h"
#include "icommand.h"

#include <QLabel>
#include <QPoint>

class AddBlockCommand : public ICommand {
public:
  AddBlockCommand(BlockId block, QPoint pos, const QString &text,
                  FieldModel &field_model, SelectionModel &selection_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  BlockId _block;
  FieldModel::BlockData _block_data;
  FieldModel &_field_model;
  SelectionModel &_selection_model;
};
