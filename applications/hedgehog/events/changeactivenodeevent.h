#pragma once

#include "../widgets/connectnodewidget.h"
#include "event.h"

class ChangeActiveNodeEvent : public Event {
public:
  ChangeActiveNodeEvent(NodeId node, bool active);
  virtual EventType GetEventType() const override;
  NodeId GetNode();
  bool GetActivity();

private:
  NodeId _node;
  bool _active;
};