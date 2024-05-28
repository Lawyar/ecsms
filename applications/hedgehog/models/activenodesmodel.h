#pragma once

#include "../connectnodewidget.h"
#include "imodel.h"

#include <map>
#include <optional>

class ActiveNodesModel : public IModel {
public:
  ActiveNodesModel() = default;
  std::map<NodeId, int> GetActiveNodes();
  void IncreaseNodeCount(const NodeId &active_node, int count = 1);
  void DecreaseNodeCount(const NodeId &active_node, int count = 1);
  int GetNodeCount(const NodeId &active_node);
  std::optional<NodeId> GetBeginOfLine() const;
  void SetBeginOfLine(std::optional<NodeId> begin);
  class Memento;
  Memento Save() const;
  void Load(const Memento &m);

  class Memento {
  public:
    Memento(const Memento &) = delete;
    Memento(Memento &&) = default;
    Memento &operator=(const Memento &) = delete;
    Memento &operator=(Memento &&) = delete;

  private:
    Memento(std::map<NodeId, int> active_nodes, std::optional<NodeId> begin)
        : _active_nodes(active_nodes), _begin(begin) {}
    std::map<NodeId, int> _active_nodes;
    std::optional<NodeId> _begin;
    friend ActiveNodesModel;
  };

private:
  std::map<NodeId, int> _active_nodes;
  std::optional<NodeId> _begin;
};
