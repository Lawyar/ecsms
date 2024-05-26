#include "changeactivenodeevent.h"

ChangeActiveNodeEvent::ChangeActiveNodeEvent(ConnectNodeWidget *node,
                                             bool active)
    : _node(node), _active(active) {}

EventType ChangeActiveNodeEvent::GetEventType() const {
  return changeActiveNodeEvent;
}

ConnectNodeWidget *ChangeActiveNodeEvent::GetNode() { return _node; }

bool ChangeActiveNodeEvent::GetActivity() { return _active; }
