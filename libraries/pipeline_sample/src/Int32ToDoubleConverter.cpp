#include "Int32ToDoubleConverter.h"

void Int32ToDoubleConverter::consumeAndProduce(std::shared_ptr<int32_t> inData,
                                      std::shared_ptr<double> outData) {
  *outData = double(*inData);
  releaseConsumerTask(inData);
  releaseProducerTask(outData, true);
}