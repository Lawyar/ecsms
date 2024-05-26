#include "blockid.h"
#include "../connectnodewidget.h"

BlockId::BlockId(int id) : Id({id}) {}

NodeId BlockId::GetNodeId(NodeType t) const { return NodeId(*this, t); }
