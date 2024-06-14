#pragma once

#include "ConsumerStage.h"
#include "DbParameterizedStage.h"

template <typename T>
class DbWriteStage : public ConsumerStage<T>, public DbParameterizedStage {
 public:
  DbWriteStage(ConsumptionStrategy strategy,
               std::shared_ptr<InStageConnection<T>>);

  void consume(std::shared_ptr<T> inData) override;

 public:
  virtual void ResetFile() override;

 public:
  static constexpr auto stageName = "DbWriteStage";
  using consumptionT = T;
  using productionT = void;
};

template <typename T>
DbWriteStage<T>::DbWriteStage(ConsumptionStrategy strategy,
                              std::shared_ptr<InStageConnection<T>> connection)
    : ConsumerStage(stageName, strategy, connection) {}

template <typename T>
void DbWriteStage<T>::consume(std::shared_ptr<T> inData) {
  if (!IsFullyParameterized()) {
    // Параметры не были заданы. Не можем писать данные.
    dataConsumed(inData, false);
    return;
  }

  if (auto status = m_connection->BeginTransaction(); status->HasError()) {
    dataConsumed(inData, false);
    return;
  }

  if (!m_file->Open(FileOpenMode::Append)) {
    dataConsumed(inData, false);
    return;
  }

  if (!m_file->WriteBytes(reinterpret_cast<char*>(inData.get()), sizeof(T))) {
    m_connection->RollbackTransaction();
    dataConsumed(inData, false);
    return;
  }

  if (auto status = m_connection->CommitTransaction(); status->HasError()) {
    dataConsumed(inData, false);
    return;
  }

  dataConsumed(inData, true);
}

template <typename T>
inline void DbWriteStage<T>::ResetFile() {
  m_file.reset();

  auto converter = GetDatabaseManager().GetSQLTypeConverter();

  m_file = m_connection->CreateRemoteFile();

  ISQLTypeRemoteFileIdPtr value =
      converter->GetSQLTypeRemoteFileId(m_file->GetFileName());

  IExecutorEAV::EntityId entityId = -1;
  if (auto status = m_executorEAV->CreateNewEntity(GetEntityName(), entityId);
      status->HasError())
    throw std::runtime_error(status->GetErrorMessage());

  if (auto status = m_executorEAV->Insert(
          GetEntityName(), entityId,
          converter->GetSQLTypeText(GetAttributeName()), value);
      status->HasError())
    throw std::runtime_error(status->GetErrorMessage());
}
