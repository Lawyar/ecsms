#include "nodeid.h"
#include "blockid.h"

NodeId::NodeId(const BlockId &id, NodeType t) : Id(id.GetChildId(t)) {}
