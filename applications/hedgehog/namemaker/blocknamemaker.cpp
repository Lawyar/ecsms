#include "blocknamemaker.h"

NameMaker::NameMaker(int counter) : counter(counter) {}

Id NameMaker::MakeName() { 
  Id id({counter}); 
  ++counter;
  return id;
}
