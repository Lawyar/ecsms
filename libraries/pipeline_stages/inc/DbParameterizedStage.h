#pragma once

#include <IParameterized.h>
#include <IExecutorEAV.h>
#include <map>

class DbParameterizedStage : public IParameterized {
  mutable std::map<std::string, std::string> m_keyValues;
  const std::vector<std::string> m_keys;
  const std::map<std::string, std::wstring> m_keysToObviousParamName;
  bool m_isApliedParams = false;  ///< Параметры заданы

protected:
  IConnectionPtr m_connection;    ///< Соединение с БД
  IExecutorEAVPtr m_executorEAV;  ///< Исполнитель EAV-запросов
  IFilePtr m_file;  ///< Большой бинарный объект, из которого читаем

 public:
  DbParameterizedStage();
  /// Получить параметры и их значения
  virtual std::vector<PatameterValue> GetPatameterValues() const override;
  /// Установить значение параметра
  virtual bool SetParameterValue(const std::string& paramName,
                                 const std::string& paramValue) override;
  /// Получить понятное название параметра
  virtual std::wstring GetObviousParamName(
      const std::string& paramName) const override;

  /// Применить значения параметров
  virtual void ApplyParameterValues() noexcept(false) override;
  /// Объект полностью (и корректно) параметризован
  virtual bool IsFullyParameterized() const override;

public:
  /// Установить переменную с большим бинарным объектом
  virtual void ResetFile() = 0;

 public:
  /// Получить строку подключения
  std::string GetConnectionInfo() const;
  /// Получить название сущности
  std::string GetEntityName() const;
  /// Получить название атрибута
  std::string GetAttributeName() const;
  /// Получить идентификатор сущности
  IExecutorEAV::EntityId GetEntityId() const;
};
