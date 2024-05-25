#pragma once

#include "drawevent.h"

#include <QPaintEvent>

class MyPaintEvent : public QPaintEvent {
public:
  MyPaintEvent(std::shared_ptr<DrawEvent> event);

  std::shared_ptr<DrawEvent> GetEvent() const;

private:
  std::shared_ptr<DrawEvent> _event;
};
