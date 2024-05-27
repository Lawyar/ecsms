#pragma once

#include "../connectnodewidget.h"
#include "imodel.h"

#include <QObject>
#include <memory>
#include <optional>

class LineModel : public QObject, public IModel {
  Q_OBJECT
public:
  LineModel() = default;
  std::optional<QPoint> GetBegin() const;
  void SetBegin(std::optional<QPoint> begin);
  std::optional<QPoint> GetEnd() const;
  void SetEnd(QPoint end);

private:
  std::optional<QPoint> _begin, _end;
};
