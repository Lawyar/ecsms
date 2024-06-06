#include "phantomrectanglemodel.h"
#include "../events/changecontrollerevent.h"

QPoint PhantomRectangleModel::GetP1() const { return _p1; }

void PhantomRectangleModel::SetP1(QPoint p1) {
  _p1 = p1;
  Notify(std::make_shared<ChangeControllerEvent>(drawRectangleController));
}

QPoint PhantomRectangleModel::GetP2() const { return _p2; }

void PhantomRectangleModel::SetP2(QPoint p2) { _p2 = p2; }
