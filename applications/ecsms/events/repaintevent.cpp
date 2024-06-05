#include "repaintevent.h"

DrawEventType RepaintEvent::GetDrawEventType() const {
  return DrawEventType::repaintEvent;
}
