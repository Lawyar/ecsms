#include "Int32ToDoubleConverter.h"

Int32ToDoubleConverter::Int32ToDoubleConverter(
    ConsumptionStrategy strategy,
    std::shared_ptr<InStageConnection<int32_t>> inConnection,
    std::shared_ptr<OutStageConnection<double>> outConnection)
    : ConsumerAndProducerStage(stageName,
                               strategy,
                               inConnection,
                               outConnection) {
}

void Int32ToDoubleConverter::consumeAndProduce(
    std::shared_ptr<int32_t> inData,
    std::shared_ptr<double> outData) {
  *outData = double(*inData);
  releaseConsumptionData(inData);
  releaseProductionData(outData, true);
}