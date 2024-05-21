#pragma once

#include <memory>
#include <string>

//------------------------------------------------------------------------------
/**
  Статус результата запроса
*/
//---
enum class ResultStatus
{
	OkWithoutData, ///< Успешное завершение команды, не возвращающей никаких данных
	OkWithData,    ///< Успешное завершение команды, возвращающей данные
	InProgress,    ///< Команда выполняется
	NonFatalError, ///< Произошла не фатальная ошибка (уведомление или предупреждение)
	FatalError,    ///< Произошла фатальная ошибка
	EmptyQuery,    ///< Строка запроса, отправленная серверу, была пустой
	Unknown        ///< Неизвестный статус
};


//------------------------------------------------------------------------------
/**
  Интерфейс статуса результата запроса к базе данных.
*/
//---
class IExecuteResultStatus
{
public:
	/// Деструктор
	virtual ~IExecuteResultStatus() = default;

public:
	/// Получить статус запроса
	virtual ResultStatus GetStatus() const = 0;
	/// Получить сообщение об ошибке, связанное с командой (возвращает пустую строку, если ошибки не произошло)
	virtual std::string GetErrorMessage() const = 0;
	/// Ответ содержит ошибку или предупреждение
	bool HasError() const
	{
		ResultStatus status = GetStatus();
		return status == ResultStatus::EmptyQuery || status == ResultStatus::NonFatalError || status == ResultStatus::FatalError || status == ResultStatus::Unknown;
	}
};

/// Указатель на IExecuteResultStatus
using IExecuteResultStatusPtr = std::shared_ptr<IExecuteResultStatus>;
