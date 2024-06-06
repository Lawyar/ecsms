#pragma once

#include "imodel.h"

#include <QPoint>

class PhantomRectangleModel : public IModel {
public:
  PhantomRectangleModel() = default;
  QPoint GetP1() const;
  void SetP1(QPoint p1);
  QPoint GetP2() const;
  void SetP2(QPoint p2);

private:
  QPoint _p1, _p2;
};
