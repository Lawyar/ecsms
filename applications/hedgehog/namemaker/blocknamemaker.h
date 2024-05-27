#pragma once

#include "id.h"

class NameMaker {
public:
  NameMaker(int counter = 0);
  Id MakeName();

private:
  int counter = 0;
};
