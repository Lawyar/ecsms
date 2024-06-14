#pragma once

#include "DbParameterizedStage.h"
#include "ProducerStage.h"

#include <exception>

#include <IDatabaseManager.h>

template <typename T>
class DbReadStage : public ProducerStage<T>, public DbParameterizedStage {
 public:
  DbReadStage(std::shared_ptr<OutStageConnection<T>>);

  void produce(std::shared_ptr<T> outData) override;

 public:
  virtual void ResetFile() override;

 public:
  static constexpr auto stageName = "DbReadStage";
  using consumptionT = void;
  using productionT = T;
};

template <typename T>
DbReadStage<T>::DbReadStage(std::shared_ptr<OutStageConnection<T>> connection)
    : ProducerStage(stageName, connection) {}

template <typename T>
void DbReadStage<T>::produce(std::shared_ptr<T> outData) {
  if (!IsFullyParameterized()) {
    // Параметры не были заданы. Не можем читать данные.
    dataProduced(outData, false);
    return;
  }

  // readSuccess указывает на то, что данные были успешно прочитаны из бд
  bool readSuccess = false;

  readSuccess =
      m_file->ReadBytes(reinterpret_cast<char*>(outData.get()), sizeof(T));

  dataProduced(outData, readSuccess);
}

template <typename T>
inline void DbReadStage<T>::ResetFile() {
  m_file.reset();

  auto converter = GetDatabaseManager().GetSQLTypeConverter();
  ISQLTypeRemoteFileIdPtr value = converter->GetSQLTypeRemoteFileId();
  auto status = m_executorEAV->GetValue(
      GetEntityName(), GetEntityId(), converter->GetSQLTypeText(GetAttributeName()),
      value);
  if (status->HasError())
    throw std::runtime_error(status->GetErrorMessage());

  m_file = m_connection->GetRemoteFile(*value->GetValue());

  m_connection->BeginTransaction();
  if (!m_file->Open(FileOpenMode::Read))
    throw std::runtime_error("Can't open binary large object");
}
