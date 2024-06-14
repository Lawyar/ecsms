#include "DbParameterizedStage.h"

#include <IDatabaseManager.h>

DbParameterizedStage::DbParameterizedStage()
    : m_keys({"connectionInfo", "entityName", "attributeName", "entityId"}),
      m_keysToObviousParamName(
          {{"connectionInfo", L"Строка подключения"},
           {"entityName", L"Название сохраняемой сущности"},
           {"attributeName", L"Название атрибута сущности"},
           {"entityId", L"Идентификатор сущности"}}) {
}

std::vector<IParameterized::PatameterValue>
DbParameterizedStage::GetPatameterValues() const {
  std::vector<PatameterValue> res;
  for (auto&& key : m_keys) {
    res.push_back({key, m_keyValues[key]});
  }
  return res;
}

bool DbParameterizedStage::SetParameterValue(const std::string& paramName,
                                             const std::string& paramValue) {
  if (std::find(m_keys.begin(), m_keys.end(), paramName) == m_keys.end())
    // Такого ключа нет
    return false;

  m_keyValues[paramName] = paramValue;
  return true;
}

std::wstring DbParameterizedStage::GetObviousParamName(
    const std::string& paramName) const {
  if (auto iter = m_keysToObviousParamName.find(paramName);
      iter != m_keysToObviousParamName.end())
    return iter->second;
  return L"<Error : unknown parameter name>";
}

void DbParameterizedStage::ApplyParameterValues() noexcept(false) {
  if (m_isApliedParams) {
    m_isApliedParams = false;
    m_connection.reset();
    m_executorEAV.reset();
    m_file.reset();
  }

  auto connectionInfo = GetConnectionInfo();
  auto entityName = GetEntityName();
  auto attributeName = GetAttributeName();
  auto entityId = GetEntityId();

  auto&& dbManager = GetDatabaseManager();
  m_connection = dbManager.GetConnection(connectionInfo);
  m_executorEAV = dbManager.GetExecutorEAV(m_connection);
  if (!m_connection || !m_connection->IsValid() || !m_executorEAV)
    throw std::runtime_error("Bad connectionInfo: \"" + connectionInfo + "\"");
  auto converter = dbManager.GetSQLTypeConverter();

  IExecutorEAV::EAVRegisterEntries entries({{entityName, {SQLDataType::RemoteFileId}}});
  if (auto status = m_executorEAV->SetRegisteredEntities(entries, true);
      status->HasError())
    throw std::runtime_error(status->GetErrorMessage());

  ResetFile();

  m_isApliedParams = true;
}

bool DbParameterizedStage::IsFullyParameterized() const {
  return m_isApliedParams;
}

std::string DbParameterizedStage::GetConnectionInfo() const {
  return m_keyValues["connectionInfo"];
}

std::string DbParameterizedStage::GetEntityName() const {
  return m_keyValues["entityName"];
}

std::string DbParameterizedStage::GetAttributeName() const {
  return m_keyValues["attributeName"];
}

IExecutorEAV::EntityId DbParameterizedStage::GetEntityId() const {
  return std::stoi(m_keyValues["entityId"]);
}
