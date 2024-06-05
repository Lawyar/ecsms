#include "changeactivenodeevent.h"

ChangeActiveNodeEvent::ChangeActiveNodeEvent(NodeId node,
                                             bool active)
    : _node(node), _active(active) {}

EventType ChangeActiveNodeEvent::GetEventType() const {
  return changeActiveNodeEvent;
}

NodeId ChangeActiveNodeEvent::GetNode() { return _node; }

bool ChangeActiveNodeEvent::GetActivity() { return _active; }
