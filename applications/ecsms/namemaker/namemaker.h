#pragma once

#include "id.h"

class NameMaker {
public:
  NameMaker(int counter = 0);
  Id MakeName();
  int GetCounter() const;
  void SetCounter(int counter);

private:
  int _counter = 0;
};
