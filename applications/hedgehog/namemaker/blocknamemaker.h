#pragma once

#include "id.h"

class BlockNameMaker {
public:
  BlockNameMaker(int counter = 0);
  Id MakeName();

private:
  int counter = 0;
};
