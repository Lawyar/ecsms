#pragma once

#include "../models/fieldmodel.h"
#include "command/addblockcommand.h"

#include <QPoint>

namespace controller::execute {
void AddBlock(NameMaker &block_name_maker, FieldModel &field_model,
              SelectionModel& selection_model,
              QPoint pos,
              const QString& block_name,
              std::shared_ptr<CommandManager> cm) {

  cm->Do(std::make_unique<AddBlockCommand>(block_name_maker.MakeName(), pos, block_name,
                             field_model, selection_model));
}
} // namespace controller::execute
