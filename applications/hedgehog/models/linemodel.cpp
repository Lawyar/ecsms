#include "linemodel.h"
#include "../events/changecontrollerevent.h"
#include "../events/drawlineevent.h"

ConnectNodeWidget *LineModel::GetBegin() { return _selected_node; }

void LineModel::SetBegin(ConnectNodeWidget *begin) {
  if (_selected_node != begin)
    _selected_node = begin;

  if (begin)
    Notify(std::make_shared<ChangeControllerEvent>(
        ControllerType::drawLineController));
  else // set start as nullptr means that user click on field and
       // doesn't end connection
    Notify(std::make_shared<ChangeControllerEvent>(
        ControllerType::defaultController));
  
}

const QPoint LineModel::GetEnd() const { return _end; }

void LineModel::SetEnd(QPoint end) {
  if (end != _end) {
    _end = end;
    Notify(std::make_shared<DrawLineEvent>(_selected_node->coordToParent(), _end));
  }
}

void LineModel::on_start(ConnectNodeWidget *start) {}