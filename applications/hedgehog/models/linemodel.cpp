#include "linemodel.h"
#include "../events/changecontrollerevent.h"
#include "../events/repaintevent.h"

std::optional<QPoint> LineModel::GetBegin() const { return _begin; }

void LineModel::SetBegin(std::optional<QPoint> begin) {
  if (_begin != begin)
    _begin = begin;
  
  ControllerType type;
  if (begin) {
    type = ControllerType::drawLineController;
    _end = std::nullopt;
  } else // set start as nullptr means that user click on field and doesn't end connection
  {
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
