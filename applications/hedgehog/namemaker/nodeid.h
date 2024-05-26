#pragma once

#include "id.h"

enum NodeType { Incoming, Outgoing };

class BlockId;

class NodeId : public Id {
public:
  NodeId(const BlockId &id, NodeType t);
};
