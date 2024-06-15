#pragma once

#include <optional>
#include <string>
#include <vector>

/// Интерфейс параметризуемого объекта
/// Параметризация идет с помощью пар "ключ-значение",
/// где "ключ" - название параметра, а "значение" - значение параметра
class IParameterized {
 public:
  struct PatameterValue {
    std::string paramName;   ///< Название параметра
    std::string paramValue;  ///< Значение параметра
  };

 public:
  virtual ~IParameterized() = default;

  /// Получить параметры и их значения
  virtual std::vector<PatameterValue> GetPatameterValues() const = 0;
  /// Установить значение параметра
  virtual bool SetParameterValue(const std::string& paramName,
                                 const std::string& paramValue) = 0;
  /// Применить значения параметров. Возвращает ошибку через исключение.
  virtual void ApplyParameterValues() noexcept(false) = 0;

  /// Получить понятное название параметра
  virtual std::optional<std::wstring> GetObviousParamName(
      const std::string& paramName) const = 0;

  /// Объект полностью (и корректно) параметризован
  virtual bool IsFullyParameterized() const = 0;

  /// Получить справочную информацию
  virtual std::wstring GetHelpString() const { return L""; }
};
