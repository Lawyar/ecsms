#include "drawlineevent.h"

DrawLineEvent::DrawLineEvent(QPoint begin, QPoint end)
    : _begin(begin), _end(end) {}

DrawEventType DrawLineEvent::GetDrawEventType() const {
  return DrawEventType::Line;
}

const QLine DrawLineEvent::GetLine() const { return QLine(_begin, _end); }
