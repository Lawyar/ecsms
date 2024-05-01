#pragma once

#include <IExecuteResultStatus.h>

#include <string>

#include <libpq-fe.h>

//------------------------------------------------------------------------------
/**
  Класс для статуса результата запроса к базе данных PostgreSQL.
*/
//---
class PGExecuteResultStatus : public IExecuteResultStatus
{
private:
	/// Внутрення информация об ошибке (представление об ошибке, генерируемое этим модулем)
	struct InternalInfo
	{
		ResultStatus status;      ///< Внутренний статус запроса
		std::string errorMessage; ///< Внутреннее сообщение об ошибке
	};

	/// Внешняя информация об ошибке (представление об ошибке, генерируемое libpq)
	struct ExternalInfo
	{
		ExecStatusType status;          ///< Статус запроса PostgreSQL
		std::string errorMessage;       ///< Строка с ошибкой
	};

private:
	const ExternalInfo m_externalInfo; ///< Внешняя информация об ошибке на момент создания объекта
	const InternalInfo m_internalInfo; ///< Внутренняя информация об ошибке на момент создания объекта
	                                   ///  (должен идти ниже m_externalInfo, поскольку при инициализации использует его поле)
	
public:
	/// Конструктор
	PGExecuteResultStatus(const PGresult * pgResult);

public:
	/// Получить статус запроса
	virtual ResultStatus GetStatus() const override;
	/// Получить сообщение об ошибке, связанное с командой (возвращает пустую строку, если ошибки не произошло)
	virtual std::string GetErrorMessage() const override;

private:
	/// Создать внутреннее представление ошибки
	static PGExecuteResultStatus::InternalInfo createInternalInfo(ExecStatusType pqStatus);
};
