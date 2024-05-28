#pragma once

#include "../models/fieldmodel.h"
#include "command/addblockcommand.h"

#include <QPoint>

namespace controller::execute {
void AddBlock(NameMaker &block_name_maker, FieldModel &field_model,
              SelectionModel &selection_model, QPoint pos,
              /*BlockType blockType,*/ std::shared_ptr<CommandManager> cm) {

  cm->Do(new AddBlockCommand(block_name_maker.MakeName(), pos, "default_block",
                             field_model, selection_model));
}
} // namespace controller::execute
