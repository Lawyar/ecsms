#include "PipelineHelpers.h"
#include "PipelineException.h"

#include <string>

using namespace std;
using namespace string_literals;

PipelineStageType PipelineHelpers::stagetTypeFromString(const string_view s) {
  if (s == "consumer")
    return PipelineStageType::consumer;
  else if (s == "producer")
    return PipelineStageType::producer;
  else if (s == "producerConsumer")
    return PipelineStageType::producerConsumer;
  else
    throw PipelineException("cannot convert string "s + s.data() +
                            " to PipelineStageType enum");
}

string PipelineHelpers::toString(PipelineStageType t) {
  switch (t) {
    case PipelineStageType::consumer:
      return "consumer";
    case PipelineStageType::producer:
      return "producer";
    case PipelineStageType::producerConsumer:
      return "producerConsumer";
    default:
      throw PipelineException("cannot convert enum with value "s + to_string(int(t)) +
                              " to corresponding string");
  }
}

ConsumptionStrategy PipelineHelpers::strategyFromString(const string_view s) {
  if (s == "fifo")
    return ConsumptionStrategy::fifo;
  else if (s == "lifo")
    return ConsumptionStrategy::lifo;
  else
    throw PipelineException("cannot convert string "s + s.data() +
                            " to ConsumptionStrategy enum");
}

string PipelineHelpers::toString(ConsumptionStrategy t) {
  switch (t) {
    case ConsumptionStrategy::fifo:
      return "fifo";
    case ConsumptionStrategy::lifo:
      return "lifo";
    default:
      throw PipelineException("cannot convert enum with value "s +
                              to_string(int(t)) + " to corresponding string");
  }
}
