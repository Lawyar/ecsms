#include "Int32RandomGeneratorPipelineFactory.h"
#include "PipelineStageObserver.h"

#include <iostream>
#include <Int32RandomGenerator.h>

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

//bool connectable(const string_view producerName,
//    const string_view consumerName) {
//  
//}

template<typename Producer, typename Consumer>
bool connectable() {
  static_assert(Producer::stageType == PipelineStageType::producer ||
                Producer::stageType == PipelineStageType::producerConsumer);

  static_assert(Consumer::stageType == PipelineStageType::consumer ||
                Consumer::stageType == PipelineStageType::producerConsumer);

  return is_same_v<Producer::productionT, Consumer::consumptionT>;
}

void observe() {
  vector<string> stageNames = { "Int32RandomGenerator, stage2" };
  cout << "Available stages:\n";
  for (size_t i = 0; i < stageNames.size(); ++i) {
    cout << to_string(i + 1) << ". " << stageNames[i] << endl;
  }

  string selectedStageName = "Int32RandomGenerator";

  if (selectedStageName == "Int32RandomGenerator") {
    auto connection =
        make_shared<InOutStageConnection<typename Int32RandomGenerator::productionT>>(32);
    Int32RandomGenerator stage(connection);

    selectedStageName = "Int32Visualizer";
    if (selectedStageName == "Int32Visualizer") {
      
    }
  }
}
