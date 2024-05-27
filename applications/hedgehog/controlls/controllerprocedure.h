#pragma once

#include "../models/fieldmodel.h"

#include <QPoint>

namespace controller::execute {
void AddBlock(NameMaker &block_name_maker,
              FieldModel &field_model,
              QPoint pos/*, BlockType blockType*/) { // TODO передавать commandmanager
  std::unique_ptr<BlockWidget> blockWidget;
  /*switch (blockType) {
  case xxx: blockWidget = new BlockWidgetXXX();
    break;
  case yyy:
    blockWidget = new BlockWidgetYYY();
    break;
  }*/
  std::unique_ptr<IController> controller;
  blockWidget.reset(new BlockWidget(block_name_maker.MakeName(), controller));
  auto &&left_p = blockWidget->GetLeftNode()->getCenterCoord();
  auto &&right_p = blockWidget->GetRightNode()->getCenterCoord();

  FieldModel::BlockData block_data = {pos,
                                      {
                                          {NodeType::Incoming, left_p},
                                          {NodeType::Outgoing, right_p},
                                      }};
  QMap<NodeType, FieldModel::NodeData> node_data_map = {
      {NodeType::Incoming, {NodeType::Incoming}},
      {NodeType::Outgoing, {NodeType::Outgoing}}};
  field_model.AddBlock(blockWidget->GetId(), block_data, node_data_map);
}
}
