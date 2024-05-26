#include "blocknamemaker.h"

BlockNameMaker::BlockNameMaker(int counter) : counter(counter) {}

Id BlockNameMaker::MakeName() { 
  Id id({counter}); 
  ++counter;
  return id;
}
