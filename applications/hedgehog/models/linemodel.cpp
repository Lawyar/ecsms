#include "linemodel.h"
#include "../events/changecontrollerevent.h"
#include "../events/repaintevent.h"

ConnectNodeWidget *LineModel::GetBegin() { return _selected_node; }

void LineModel::SetBegin(ConnectNodeWidget *begin) {
  if (_selected_node != begin)
    _selected_node = begin;

  if (begin) {
    Notify(std::make_shared<ChangeControllerEvent>(
        ControllerType::drawLineController));
    _end = begin->getCenterCoordToBlockField();
  }
  else // set start as nullptr means that user click on field and
       // doesn't end connection
    Notify(std::make_shared<ChangeControllerEvent>(
        ControllerType::defaultController));
  Notify(std::make_shared<RepaintEvent>());
}

const QPoint LineModel::GetEnd() const { return _end; }

void LineModel::SetEnd(QPoint end) {
  if (end != _end) {
    _end = end;
    Notify(std::make_shared<RepaintEvent>());
  }
}

void LineModel::on_start(ConnectNodeWidget *start) {}
