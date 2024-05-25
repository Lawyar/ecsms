#include "mypaintevent.h"

#include <QRect>

MyPaintEvent::MyPaintEvent(std::shared_ptr<DrawEvent> event)
    : _event(event), QPaintEvent(QRect(-1000, -1000, 2000, 2000)) {
  // TODO: count rect
}

std::shared_ptr<DrawEvent> MyPaintEvent::GetEvent() const { return _event; }
