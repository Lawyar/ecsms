#pragma once

#include "ProducerStage.h"

template<typename T>
class DbReadStage : public ProducerStage<T> {
public:
    DbReadStage(std::shared_ptr<InStageConnection<T>>);

    void produce(std::shared_ptr<T> outData) override;

 public:
  static constexpr auto stageName = "DbReadStage";
  using consumptionT = void;
  using productionT = T;
};

template<typename T>
DbReadStage<T>::DbReadStage(std::shared_ptr<InStageConnection<T>> connection) : ProducerStage(stageName, connection) {}

template<typename T>
void DbReadStage<T>::produce(std::shared_ptr<T> outData) {
  // readSuccess указывает на то, что данные были успешно прочитаны из бд
  bool readSuccess = true;

  // Здесь должно быть чтение из бд в переменную outData;
  // *outData = dataFromDatabase;

  dataProduced(outData, readSuccess);
}
