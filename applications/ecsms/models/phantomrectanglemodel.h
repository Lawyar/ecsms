#pragma once

#include "imodel.h"

#include <QPoint>
#include <QRect>
#include <optional>

class PhantomRectangleModel : public IModel {
public:
  PhantomRectangleModel() = default;

  PhantomRectangleModel &operator=(const PhantomRectangleModel &other);

  std::optional<QPoint> GetP1() const;
  void SetP1(std::optional<QPoint> p1);
  std::optional<QPoint> GetP2() const;
  void SetP2(QPoint p2);
  bool ContainsRect(QRect r) const;

  void Clear();

private:
  std::optional<QPoint> _p1, _p2;
};
