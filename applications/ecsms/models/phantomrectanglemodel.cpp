#include "phantomrectanglemodel.h"
#include "../events/changecontrollerevent.h"
#include "../events/repaintevent.h"

std::optional<QPoint> PhantomRectangleModel::GetP1() const { return _p1; }

void PhantomRectangleModel::SetP1(QPoint p1) {
  _p1 = p1;
  Notify(std::make_shared<ChangeControllerEvent>(drawRectangleController));
}

std::optional<QPoint> PhantomRectangleModel::GetP2() const { return _p2; }

void PhantomRectangleModel::SetP2(std::optional<QPoint> p2) {
  _p2 = p2;
  if (!_p2) {
    _p1 = std::nullopt;
    Notify(std::make_shared<ChangeControllerEvent>(defaultController));
  }
  Notify(std::make_shared<RepaintEvent>());
}

bool PhantomRectangleModel::ContainsRect(QRect r) const {
  if (!_p1 || !_p2) {
    return false;
  }

  QRect rect(*_p1, *_p2);
  return rect.contains(r);
}
