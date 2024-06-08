#include "phantomrectanglemodel.h"
#include "../events/changecontrollerevent.h"
#include "../events/repaintevent.h"

PhantomRectangleModel &
PhantomRectangleModel::operator=(const PhantomRectangleModel &other) {
  if (&other == this)
    return *this;

  Clear();

  _p1 = other._p1;
  _p2 = other._p2;

  IModel::operator=(other);

  return *this;
}

std::optional<QPoint> PhantomRectangleModel::GetP1() const { return _p1; }

void PhantomRectangleModel::SetP1(std::optional<QPoint> p1) {
  _p1 = p1;
  if (!_p1) {
    _p2 = std::nullopt;
    Notify(std::make_shared<ChangeControllerEvent>(defaultController));
  }
  Notify(std::make_shared<RepaintEvent>());
}

std::optional<QPoint> PhantomRectangleModel::GetP2() const { return _p2; }

void PhantomRectangleModel::SetP2(QPoint p2) {
  _p2 = p2;
  Notify(std::make_shared<ChangeControllerEvent>(drawRectangleController));
}

bool PhantomRectangleModel::ContainsRect(QRect r) const {
  if (!_p1 || !_p2) {
    return false;
  }

  QRect rect(*_p1, *_p2);
  return rect.contains(r);
}

void PhantomRectangleModel::Clear() {
  _p1.reset();
  _p1 = std::nullopt;
  _p2.reset();
  _p2 = std::nullopt;
  Notify(std::make_shared<RepaintEvent>());
  IModel::Clear();
}
