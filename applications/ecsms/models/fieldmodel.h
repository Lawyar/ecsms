#pragma once

#include "../namemaker/id.h"
#include "../namemaker/namemaker.h"
#include "../widgets/connectnodewidget.h"
#include "imodel.h"
#include "yaml-cpp/yaml.h"

#include <QMap>
#include <optional>

class FieldModel : public IModel {
 public:
  struct BlockData {
    QPoint pos;
    QSize size;
    std::map<NodeType, QPoint> offset;
    QString text;
  };
  struct NodeData {
    NodeType node_type;
  };

  class Memento {
   public:
    Memento(const Memento&) = delete;
    Memento(Memento&&) = default;

    Memento& operator=(const Memento&) = delete;
    Memento& operator=(Memento&&) = delete;

   private:
    Memento(QMap<NodeId, std::vector<NodeId>> connections,
            QMap<BlockId, BlockData> blocks,
            QMap<NodeId, NodeData> nodes)
        : _connections(connections), _blocks(blocks), _nodes(nodes) {}
    QMap<NodeId, std::vector<NodeId>> _connections;
    QMap<BlockId, BlockData> _blocks;
    QMap<NodeId, NodeData> _nodes;
    friend FieldModel;
  };

 public:
  FieldModel() = default;

  FieldModel& operator=(const FieldModel& other);

  const QMap<NodeId, std::vector<NodeId>>& GetConnectionMap() const;
  QMap<NodeId, std::vector<NodeId>> GetNodeConnections(NodeId node) const;

  QMap<NodeId, QMap<NodeId, std::vector<NodeId>>> GetBlockConnections(
      BlockId block) const;
  const QMap<BlockId, BlockData>& GetBlocks() const;
  std::optional<BlockData> GetBlockData(const BlockId& block) const;
  void SetBlockData(const BlockId& block, BlockData bd);

  const QMap<NodeId, NodeData>& GetNodes() const;
  std::optional<NodeData> GetNodeData(const NodeId& node) const;

  void AddConnection(const NodeId& start, const NodeId& end);
  void RemoveConnection(const NodeId& start, const NodeId& end);
  bool IsNodeConnected(const NodeId& node) const;
  void AddBlock(const BlockId& block,
                const BlockData& bd,
                const QMap<NodeType, NodeData>& node_data_map);
  void RemoveBlock(const BlockId& block);

  void Clear();

  Memento Save() const;
  void Load(const Memento& m);

 private:
  QMap<NodeId, std::vector<NodeId>> _connections;
  QMap<BlockId, BlockData> _blocks;
  QMap<NodeId, NodeData> _nodes;
};

inline static void writeIdToNode(YAML::Node& node, std::string key, Id id) {
  for (auto&& part : id.GetId())
    node[key].push_back(part);
}

namespace YAML {
template <>
struct convert<::FieldModel> {
  static Node encode(const ::FieldModel& f) {
    Node main, blocks;
    auto&& blocks_map = f.GetBlocks();
    for (auto&& pair_it = blocks_map.begin(); pair_it != blocks_map.end();
         ++pair_it) {
      auto&& block_id = pair_it.key();
      auto&& block_data = pair_it.value();

      Node block_node;

      block_node["id"] = block_id.GetId();

      block_node["pos"] =
          std::vector<int>{block_data.pos.x(), block_data.pos.y()};

      block_node["size"] =
          std::vector<int>{block_data.size.width(), block_data.size.height()};

      Node offset_node;
      auto pos = block_data.offset.at(::NodeType::Incoming);
      offset_node["incoming"] = std::vector<int>{pos.x(), pos.y()};

      pos = block_data.offset.at(::NodeType::Outgoing);
      offset_node["outgoing"] = std::vector<int>{pos.x(), pos.y()};

      block_node["offset"] = offset_node;

      block_node["text"] = block_data.text.toStdString();

      main["blocks"].push_back(block_node);
    }

    Node connections;
    auto&& connections_map = f.GetConnectionMap();
    for (auto&& pair_it = connections_map.begin();
         pair_it != connections_map.end(); ++pair_it) {
      Node connection_node;

      auto&& start_node_id = pair_it.key();
      connection_node["start_node_id"] = start_node_id.GetId();

      for (auto&& end_node_id : pair_it.value()) {
        connection_node["end_node_id"].push_back(end_node_id.GetId());
      }
      main["connections"].push_back(connection_node);
    }

    return main;
  }

  static bool decode(const Node& node, ::FieldModel& f) {
    for (auto&& block : node["blocks"]) {
      std::vector<int> id = block["id"].as<std::vector<int>>();
      BlockId block_id(id);

      auto&& pos_v = block["pos"].as<std::vector<int>>();
      QPoint pos(pos_v[0], pos_v[1]);

      auto&& size_v = block["size"].as<std::vector<int>>();
      QSize size(size_v[0], size_v[1]);

      std::map<::NodeType, QPoint> offset;
      offset[::NodeType::Incoming] = {block["offset"]["incoming"][0].as<int>(),
                                      block["offset"]["incoming"][1].as<int>()};
      offset[::NodeType::Outgoing] = {block["offset"]["outgoing"][0].as<int>(),
                                      block["offset"]["outgoing"][1].as<int>()};

      QString text(block["text"].as<std::string>().c_str());

      FieldModel::BlockData bd{pos, size, offset, text};

      auto&& ln_id =
          block_id.GetChildId(static_cast<PartId>(::NodeType::Incoming));
      auto&& rn_id =
          block_id.GetChildId(static_cast<PartId>(::NodeType::Outgoing));
      f.AddBlock(
          block_id, bd,
          {{::NodeType::Incoming, FieldModel::NodeData{::NodeType::Incoming}},
           {::NodeType::Outgoing, FieldModel::NodeData{::NodeType::Outgoing}}});
    }

    for (auto&& connect : node["connections"]) {
      auto&& start_id = connect["start_node_id"].as<std::vector<int>>();

      for (auto&& end_id_node : connect["end_node_id"]) {
        auto&& end_id = end_id_node.as<std::vector<int>>();
        f.AddConnection(start_id, end_id);
      }
    }
    return true;
  }
};
}  // namespace YAML
