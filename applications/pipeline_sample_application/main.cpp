#include "Int32RandomGeneratorPipelineFactory.h"
#include "PipelineStageObserver.h"

#include <iostream>

using namespace std;

void observe();

int main() {
  try {
    //auto pipeline = Int32RandomGeneratorPipelineFactory::create();

    //pipeline->run();

    //while (true) {
    //  this_thread::sleep_for(chrono::milliseconds(500));
    //}
    //observe();
  } catch (std::exception &ex) {
    cerr << ex.what() << endl;
  } catch (...) {
    cerr << "unhandled exception in main thread" << endl;
  }
}

//void observe() {
//  PipelineStageObserver::init();
//  auto stages = PipelineStageObserver::getStages();
//
//  cout << "Available stages:\n";
//  for (size_t i = 0; i < stages.size(); ++i) {
//    cout << to_string(i + 1) << ". " << stages[i].stageName << endl;
//  }
//
//  cout << "Select stage to create: ";
//
//  size_t stageIndex;
//  cin >> stageIndex;
//
//  auto &selectedStage = stages[stageIndex - 1];
//  for (size_t i = 0; i < stages.size(); ++i) {
//    if (i == stageIndex - 1) {
//      continue;
//    }
//
//    InOutStageConnection<stages[i]::>
//  }
//}
