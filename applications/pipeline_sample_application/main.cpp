#include "Int32RandomGeneratorPipelineFactory.h"

#include <iostream>

using namespace std;

int main() {
  try {
    auto pipeline = Int32RandomGeneratorPipelineFactory::create();

    pipeline->run();

    while (true) {
      this_thread::sleep_for(chrono::milliseconds(500));
    }
  } catch (std::exception &ex) {
    cerr << ex.what() << endl;
  } catch (...) {
    cerr << "unhandled exception in main thread" << endl;
  }
}