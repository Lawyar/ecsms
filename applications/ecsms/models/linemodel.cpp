#include "linemodel.h"
#include "../events/changecontrollerevent.h"
#include "../events/repaintevent.h"

std::optional<QPoint> LineModel::GetBegin() const { return _begin; }

std::optional<NodeId> LineModel::GetBeginNode() const {
  if (_begin) {
    return _begin_node;
  }
  return std::nullopt;
}

void LineModel::SetBegin(std::optional<NodeId> node,
                         std::optional<QPoint> begin) {
  if (_begin_node != node) {
    _begin = begin;
    _begin_node = node;
  }

  ControllerType type;
  // set start as nullptr means that user click on field and doesn't end
  // connection
  if (begin) {
    type = ControllerType::drawLineController;
    _end = std::nullopt;
  } else {
    type = ControllerType::defaultController;
  }
  Notify(std::make_shared<ChangeControllerEvent>(type));
  Notify(std::make_shared<RepaintEvent>());
}

std::optional<QPoint> LineModel::GetEnd() const { return _end; }

void LineModel::SetEnd(QPoint end) {
  if (end != _end) {
    _end = end;
    Notify(std::make_shared<RepaintEvent>());
  }
}
