#include "Int32ToDoubleConverter.h"
#include "PipelineRegistry.h"

Int32ToDoubleConverter::Int32ToDoubleConverter(
    ConsumerStrategy strategy,
    std::shared_ptr<InStageConnection<int32_t>> inConnection,
    std::shared_ptr<OutStageConnection<double>> outConnection)
    : ConsumerAndProducerStage(stageName,
                               strategy,
                               inConnection,
                               outConnection) {
  PipelineRegistry::Instance()
      .registerConsumerAndProducer<Int32ToDoubleConverter>(stageName);
}

void Int32ToDoubleConverter::consumeAndProduce(
    std::shared_ptr<int32_t> inData,
    std::shared_ptr<double> outData) {
  *outData = double(*inData);
  releaseConsumptionData(inData);
  releaseProductionData(outData, true);
}