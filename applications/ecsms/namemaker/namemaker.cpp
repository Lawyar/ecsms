#include "namemaker.h"

NameMaker::NameMaker(int counter) : _counter(counter) {}

Id NameMaker::MakeName() { 
  Id id({_counter}); 
  ++_counter;
  return id;
}

int NameMaker::GetCounter() const {
  return _counter;
}

void NameMaker::SetCounter(int counter) {
  _counter = counter;
}
