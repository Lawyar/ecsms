#pragma once

#include <memory>
#include <string>

//------------------------------------------------------------------------------
/**
  \brief Статус результата запроса
*/
//---
enum class ResultStatus {
  OkWithoutData, ///< Успешное завершение команды, не возвращающей никаких
                 ///< данных
  OkWithData, ///< Успешное завершение команды, возвращающей данные
  InProgress, ///< Команда выполняется
  NonFatalError, ///< Произошла не фатальная ошибка (уведомление или
                 ///< предупреждение)
  FatalError,    ///< Произошла фатальная ошибка
  EmptyQuery, ///< Строка запроса, отправленная серверу, была пустой
  Unknown     ///< Неизвестный статус
};

//------------------------------------------------------------------------------
/**
  \brief Интерфейс статуса результата запроса к базе данных.
*/
//---
class IExecuteResultStatus {
public:
  /// Деструктор
  virtual ~IExecuteResultStatus() = default;

public:
  /// Получить статус запроса
  /// \return Статус запроса
  virtual ResultStatus GetStatus() const = 0;
  /// Получить сообщение об ошибке, связанное с командой
  /// \return Строку с ошибкой, если произошла ошибка, иначе пустую строку.
  virtual std::string GetErrorMessage() const = 0;
  /// Проверить, содержит ли результат ошибку или предупреждение
  /// \return \c true, если результат содержит ошибку или предупреждение,
  ///         иначе \c false.
  bool HasError() const {
    ResultStatus status = GetStatus();
    return status == ResultStatus::EmptyQuery ||
           status == ResultStatus::NonFatalError ||
           status == ResultStatus::FatalError ||
           status == ResultStatus::Unknown;
  }
};

/// Указатель на IExecuteResultStatus
using IExecuteResultStatusPtr = std::shared_ptr<IExecuteResultStatus>;
