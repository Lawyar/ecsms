#pragma once

#include "DbParameterizedStage.h"
#include "ProducerStage.h"

#include <exception>

#include <IDatabaseManager.h>

template <typename T>
class DbReadStage : public ProducerStage<T>, public DbParameterizedStage {
  mutable std::map<std::string, std::string> m_keyValues;
  const std::vector<std::string> m_keys;
  const std::map<std::string, std::wstring> m_keysToObviousParamName;
  bool m_isApliedParams = false; ///< Параметры чтения заданы

 public:
  DbReadStage(std::shared_ptr<OutStageConnection<T>>);

  void produce(std::shared_ptr<T> outData) override;

 public:
  /// Получить параметры и их значения
  virtual std::vector<PatameterValue> GetPatameterValues() const override;
  /// Установить значение параметра
  virtual bool SetParameterValue(const std::string& paramName,
                                 const std::string& paramValue) override;
  /// Получить понятное название параметра
  virtual std::optional<std::wstring> GetObviousParamName(
      const std::string& paramName) const override;

  /// Применить значения параметров
  virtual void ApplyParameterValues() noexcept(false) override;
  /// Объект полностью (и корректно) параметризован
  virtual bool IsFullyParameterized() const override;

  virtual void ResetFile() override;

public:
  /// Получить идентификатор сущности для чтения
  IExecutorEAV::EntityId GetEntityId() const;

 public:
  static inline std::string stageName = std::string(typeid(T).name()) + "FromDatabase";
  using consumptionT = void;
  using productionT = T;
};

template <typename T>
DbReadStage<T>::DbReadStage(std::shared_ptr<OutStageConnection<T>> connection)
    : ProducerStage(stageName, connection),
      m_keys({"entityId"}),
      m_keysToObviousParamName(
          {{"entityId", L"Идентификатор сущности"}})
{
}

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
inline std::vector<IParameterized::PatameterValue> DbReadStage<T>::GetPatameterValues() const {
  auto res = DbParameterizedStage::GetPatameterValues();
  for (auto&& key : m_keys) {
    res.push_back({key, m_keyValues[key]});
  }
  return res;
}

template <typename T>
inline bool DbReadStage<T>::SetParameterValue(const std::string& paramName,
                                             const std::string& paramValue) {
  if (DbParameterizedStage::SetParameterValue(paramName, paramValue))
    return true;

  if (std::find(m_keys.begin(), m_keys.end(), paramName) == m_keys.end())
    // Такого ключа нет
    return false;
  m_keyValues[paramName] = paramValue;
  return true;
}

template<typename T>
inline std::optional<std::wstring> DbReadStage<T>::GetObviousParamName(
    const std::string& paramName) const {
  if (auto res = DbParameterizedStage::GetObviousParamName(paramName))
    return res;

  if (auto iter = m_keysToObviousParamName.find(paramName);
      iter != m_keysToObviousParamName.end())
    return iter->second;
  return std::nullopt;
}

template <typename T>
inline void DbReadStage<T>::ApplyParameterValues() noexcept(false) {
  m_isApliedParams = false;
  DbParameterizedStage::ApplyParameterValues();
  auto id = GetEntityId();  // Если не задан, то кинет исключение
  m_isApliedParams = true;
  // setId("EntityName: " + GetEntityName() + "; EntityId: " + std::to_string(id));
}

template <typename T>
inline bool DbReadStage<T>::IsFullyParameterized() const {
  return DbParameterizedStage::IsFullyParameterized() && m_isApliedParams;
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


template <typename T>
inline IExecutorEAV::EntityId DbReadStage<T>::GetEntityId() const
{
  return std::stoi(m_keyValues["entityId"]);
}
