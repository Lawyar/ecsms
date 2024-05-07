#include "foo.h"

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
  ecsms::foo f;
  f.doFoo();
  cout << f.getFoos() << endl;
}
