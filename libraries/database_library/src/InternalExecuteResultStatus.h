#pragma once

#include <IExecuteResultStatus.h>

#include <string>

//------------------------------------------------------------------------------
/**
  Сообщения для повторяющихся ошибок
*/
//---
struct ErrorMessages
{
	static constexpr char * ISQLTypeConverter_GetSQLVariable = "ISQLTypeConverter::GetSQLVariable: Failed to get SQL-variable";
	static constexpr char * IConnection_Execute = "IConnection::Execute: Failed to execute query";
	static constexpr char * IExecuteResult_GetCurrentExecuteStatus = "IExecuteResult::GetCurrentExecuteStatus: Failed to get execute status";
	static constexpr char * ISQLType_ReadFromSQL = "ISQLType::ReadFromSQL: Failed to read data into SQL-variable";
	static constexpr char * ISQLType_GetValue = "ISQLType::GetValue: Failed to get value from SQL-variable";
};


//------------------------------------------------------------------------------
/**
  Класс для статуса результата запроса, сигнализирующего об внутренней ошибке,
  произошедшей в этом модуле
  Или для успешного статуса, который не может быть представлен внешним статусом.
*/
//---
class InternalExecuteResultStatus : public IExecuteResultStatus
{
private:
	/// Внутрення информация об ошибке (представление об ошибке, генерируемое этим модулем)
	struct InternalInfo
	{
		ResultStatus status;      ///< Внутренний статус запроса
		std::string errorMessage; ///< Внутреннее сообщение об ошибке
	};

private:
	const InternalInfo m_internalInfo; ///< Внутренняя информация об ошибке на момент создания объекта

public:
	/// Конструктор
	InternalExecuteResultStatus(ResultStatus status, const std::string & errorMessage);

public:
	/// Получить статус запроса
	virtual ResultStatus GetStatus() const override;
	/// Получить сообщение об ошибке, связанное с командой (возвращает пустую строку, если ошибки не произошло)
	virtual std::string GetErrorMessage() const override;

public:
	/// Получить статус внутренней ошибки
	static IExecuteResultStatusPtr GetInternalError(const std::string & errorMessage = "Unknown error",
		ResultStatus status = ResultStatus::FatalError);
	/// Получить успешный статус
	static IExecuteResultStatusPtr GetSuccessStatus(ResultStatus status = ResultStatus::OkWithData);

};

