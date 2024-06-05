#pragma once

#include <algorithm>
#include <vector>

#include "../events/event.h"
#include "../observer/iobserver.h"

class IModel {
public:
  virtual ~IModel() = default;
  virtual void Notify(std::shared_ptr<Event> e) {
    for (auto &&o : _observers) {
      o->Update(e);
    }
  }

  void Subscribe(IObserver *observer) {
    _observers.push_back(observer);
  }

  void Unsubscribe(IObserver *observer) {
    _observers.erase(std::find(_observers.begin(), _observers.end(), observer));
  }

protected:
  std::vector<IObserver *> _observers;
};
