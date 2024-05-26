#pragma once

#include "../connectnodewidget.h"
#include "event.h"

class ChangeActiveNodeEvent : public Event {
public:
  ChangeActiveNodeEvent(ConnectNodeWidget *node, bool active);
  virtual EventType GetEventType() const override;
  ConnectNodeWidget *GetNode();
  bool GetActivity();

private:
  ConnectNodeWidget *_node;
  bool _active;
};