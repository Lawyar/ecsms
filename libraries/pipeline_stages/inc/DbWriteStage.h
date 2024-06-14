#pragma once

#include "ConsumerStage.h"

template<typename T>
class DbWriteStage : public ConsumerStage<T> {
public:
    DbWriteStage(ConsumptionStrategy strategy,
                 std::shared_ptr<InStageConnection<T>>);

    void consume(std::shared_ptr<T> inData) override;
 public:
  static constexpr auto stageName = "DbWriteStage";
  using consumptionT = T;
  using productionT = void;
};

template<typename T>
DbWriteStage<T>::DbWriteStage(ConsumptionStrategy strategy,
             std::shared_ptr<InStageConnection<T>> connection) : ConsumerStage(stageName, strategy, connection)
    {}

template<typename T>
void DbWriteStage<T>::consume(std::shared_ptr<T> inData) {
  // writeSuccess указывает на то, что данные были успешно записаны в бд
  bool writeSuccess = true;

  // Здесь должна быть запись в БД данных *inData; *inData нулевым быть не может

  dataConsumed(inData, writeSuccess);
}
