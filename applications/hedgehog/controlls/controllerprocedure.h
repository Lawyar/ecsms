#pragma once

#include "../models/fieldmodel.h"

#include <QPoint>

namespace ControllerProcedure {
namespace Execute {
void AddBlock(NameMaker &block_name_maker,
              std::unique_ptr<IController> &controller,
              BlockField *parent, FieldModel &field_model,
              QPoint pos) {
  auto b = new BlockWidget(block_name_maker.MakeName(), controller, parent);
  b->show();
  b->move(pos);
  auto &&left_p = b->GetLeftNode()->getCenterCoord();
  auto &&right_p = b->GetRightNode()->getCenterCoord();

  FieldModel::BlockData block_data = {b->pos(),
                                      {
                                          {NodeType::Incoming, left_p},
                                          {NodeType::Outgoing, right_p},
                                      }};
  QMap<NodeType, FieldModel::NodeData> node_data_map = {
      {NodeType::Incoming, {NodeType::Incoming}},
      {NodeType::Outgoing, {NodeType::Outgoing}}};
  field_model.AddBlock(b->GetId(), block_data, node_data_map);
}
}; // namespace Execute
}; // namespace ControllerProcedure
