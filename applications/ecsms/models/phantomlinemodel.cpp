#include "phantomlinemodel.h"
#include "../events/changecontrollerevent.h"
#include "../events/repaintevent.h"

PhantomLineModel &PhantomLineModel::operator=(const PhantomLineModel &other) {
  if (&other == this)
    return *this;

  Clear();

  _begin_node = other._begin_node;
  _begin = other._begin;
  _end = other._end;

  IModel::operator=(other);

  return *this;
}

std::optional<QPoint> PhantomLineModel::GetBegin() const { return _begin; }

std::optional<NodeId> PhantomLineModel::GetBeginNode() const {
  if (_begin) {
    return _begin_node;
  }
  return std::nullopt;
}

void PhantomLineModel::SetBegin(std::optional<NodeId> node,
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

std::optional<QPoint> PhantomLineModel::GetEnd() const { return _end; }

void PhantomLineModel::SetEnd(QPoint end) {
  if (end != _end) {
    _end = end;
    Notify(std::make_shared<RepaintEvent>());
  }
}

void PhantomLineModel::Clear() {
  _begin_node.reset();
  _begin_node = std::nullopt;
  _begin.reset();
  _begin = std::nullopt;
  _end.reset();
  _end = std::nullopt;
  Notify(std::make_shared<RepaintEvent>());
  IModel::Clear();
}
