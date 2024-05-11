#include "PGConnection.h"

#include <PGSQLDataTypeUtility.h>
#include <PGExecuteResult.h>
#include <PGRemoteFile.h>
#include <InternalExecuteResultStatus.h>

#include <cassert>

//------------------------------------------------------------------------------
/**
  Вспомогательная переменная для std::visit
*/
//---
template<class>
inline constexpr bool always_false_v = false;


//------------------------------------------------------------------------------
/**
  Перевести формат данных в представление, которое требует libpq
*/
//---
static int ResultFormatToPQ(ResultFormat type)
{
	constexpr int text = 0;
	constexpr int binary = 1;
	int result = 0;
	switch (type)
	{
	case ResultFormat::Text:
		result = text;
		break;
	case ResultFormat::Binary:
		result = binary;
		break;
	case ResultFormat::Invalid:
		result = text;
		fprintf(stderr, "[Warning] Passed ResultFormat::Invalid. It is treated as ResultFormat::Text.\n");
		break;
	default:
		fprintf(stderr, "[Error] Passed unknown ResultFormat value. It is treated as ResultFormat::Text.\n");
		result = text;
		break;
	}
	return result;
}


//------------------------------------------------------------------------------
/**
  Создать объект
*/
//---
PGConnectionSPtr PGConnection::Create(const std::string & connectionInfo)
{
	return PGConnectionSPtr(new PGConnection(connectionInfo));
}


//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGConnection::PGConnection(const std::string & connectionInfo)
	: m_conn(PQconnectdb(connectionInfo.c_str()))
{
}


//------------------------------------------------------------------------------
/**
  Деструктор
*/
//---
PGConnection::~PGConnection()
{
	PQfinish(m_conn);
	m_conn = nullptr;
}


//------------------------------------------------------------------------------
/**
  Валидно ли соединение
*/
//---
bool PGConnection::IsValid() const
{
	return GetStatus() == ConnectionStatus::Ok;
}

//------------------------------------------------------------------------------
/**
  Получить статус соединения
*/
//---
ConnectionStatus PGConnection::GetStatus() const
{
	ConnectionStatus status = ConnectionStatus::Unknown;

	ConnStatusType pqStatus = PQstatus(m_conn);
	switch (pqStatus)
	{
	case CONNECTION_OK:
		status = ConnectionStatus::Ok;
		break;
	case CONNECTION_BAD:
		status = ConnectionStatus::Bad;
		break;
	default:
		status = ConnectionStatus::Unknown;
		fprintf(stderr, "[Warning] Unknown connection status type : %d\n", pqStatus);
		break;
	}

	return status;
}


//------------------------------------------------------------------------------
/**
  Выполнить запрос
*/
//---
IExecuteResultPtr PGConnection::Execute(const std::string & query)
{
	std::lock_guard guard(m_mutex);
	return executeImpl(query);
}


//------------------------------------------------------------------------------
/**
  Открыть транзакцию
*/
//---
IExecuteResultStatusPtr PGConnection::BeginTransaction()
{
	auto result = Execute("BEGIN;");
	return result ? result->GetCurrentExecuteStatus() : nullptr;
}


//------------------------------------------------------------------------------
/**
  Закрыть транзакцию с применением изменений
*/
//---
IExecuteResultStatusPtr PGConnection::CommitTransaction()
{
	auto result = Execute("END;");
	return result ? result->GetCurrentExecuteStatus() : nullptr;
}


//------------------------------------------------------------------------------
/**
  Отменить транзакцию (без применения изменений)
*/
//---
IExecuteResultStatusPtr PGConnection::RollbackTransaction()
{
	auto result = Execute("ROLLBACK;");
	return result ? result->GetCurrentExecuteStatus() : nullptr;
}


//------------------------------------------------------------------------------
/**
  Вспомогательный метод для реализации Execute. Не блокирует мьютекс.
*/
//---
IExecuteResultPtr PGConnection::executeImpl(const std::string & query)
{
	PGresult * pqResult = PQexec(m_conn, query.c_str());
	IExecuteResultPtr result(new PGExecuteResult(pqResult));
	return result;
}


//------------------------------------------------------------------------------
/**
  Создать удаленный файл
*/
//---
IFilePtr PGConnection::CreateRemoteFile()
{
	Oid oid = LoCreate();
	return oid == InvalidOid ? nullptr : std::make_shared<PGRemoteFile>(weak_from_this(), oid);
}


//------------------------------------------------------------------------------
/**
  Выполнить запрос с аргументами
*/
//---
IExecuteResultPtr PGConnection::Execute(const std::string & singleCommand,
	const std::vector<ExecuteArgType> & args,
	const ResultFormat resultFormat,
	const std::vector<SQLDataType> & types)
{
	std::vector<const char*> pqParamValues;
	std::vector<int> pqParamLengths;
	std::vector<int> pqParamFormats;
	for (auto && arg : args)
	{
		std::visit([&pqParamValues, &pqParamLengths, &pqParamFormats](auto && arg)
			{
				pqParamValues.push_back(arg.data());
				pqParamLengths.push_back(static_cast<int>(arg.size()));
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::string>)
					pqParamFormats.push_back(0);
				else if constexpr (std::is_same_v<T, std::vector<char>>)
					pqParamFormats.push_back(1);
				else
					static_assert(always_false_v<T>, "Incorrect ExecuteArgType");
			}, arg);
	}

	std::vector<Oid> pqTypes;
	for (auto && type : types)
		pqTypes.push_back(SQLDataTypeToOid(type));

	int pqResultFormat = ResultFormatToPQ(resultFormat);

	std::lock_guard guard(m_mutex);
	PGresult * pqResult = PQexecParams(m_conn, singleCommand.c_str(), args.size(), pqTypes.data(),
		pqParamValues.data(), pqParamLengths.data(), pqParamFormats.data(), pqResultFormat);

	IExecuteResultPtr result(new PGExecuteResult(pqResult));
	return result;
}


//------------------------------------------------------------------------------
/**
  Создать большой бинарный объект
*/
//---
Oid PGConnection::LoCreate()
{
	std::lock_guard guard(m_mutex);
	return lo_create(m_conn, InvalidOid);
}


//------------------------------------------------------------------------------
/**
  Открыть большой бинарный объект
*/
//---
int PGConnection::LoOpen(Oid objId, int mode)
{
	std::lock_guard guard(m_mutex);
	return lo_open(m_conn, objId, mode);
}


//------------------------------------------------------------------------------
/**
  Прочитать данные из большого бинарного объекта
*/
//---
int PGConnection::LoRead(int fd, char * buffer, size_t len)
{
	std::lock_guard guard(m_mutex);	
	return lo_read(m_conn, fd, buffer, len);
}


//------------------------------------------------------------------------------
/**
  Записать данные в большой бинарный объект
*/
//---
int PGConnection::LoWrite(int fd, const char * data, size_t len)
{
	std::lock_guard guard(m_mutex);
	return lo_write(m_conn, fd, data, len);
}


//------------------------------------------------------------------------------
/**
  Закрыть большой бинарный объект
*/
//---
int PGConnection::LoClose(int fd)
{
	std::lock_guard guard(m_mutex);
	return lo_close(m_conn, fd);
}
