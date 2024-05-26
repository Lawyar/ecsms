#pragma once

#include "nodeid.h"
#include "id.h"

class BlockId : public Id {
public:
  BlockId(int id);

  NodeId GetNodeId(NodeType t) const;
};
