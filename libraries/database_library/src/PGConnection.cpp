#include "PGConnection.h"

#include <PGSQLDataTypeUtility.h>
#include <PGExecuteResult.h>
#include <PGRemoteFile.h>
#include <InternalExecuteResultStatus.h>
#include <Utils/StringUtils.h>

#include <cassert>

//------------------------------------------------------------------------------
/**
  ��������������� ���������� ��� std::visit
*/
//---
template<class>
inline constexpr bool always_false_v = false;


//------------------------------------------------------------------------------
/**
  ��������� ������ ������ � �������������, ������� ������� libpq
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
  ������� ������
*/
//---
PGConnectionSPtr PGConnection::Create(const std::string & connectionInfo)
{
	return PGConnectionSPtr(new PGConnection(connectionInfo));
}


//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGConnection::PGConnection(const std::string & connectionInfo)
	: m_conn(PQconnectdb(connectionInfo.c_str()))
{
}


//------------------------------------------------------------------------------
/**
  ����������
*/
//---
PGConnection::~PGConnection()
{
	PQfinish(m_conn);
	m_conn = nullptr;
}


//------------------------------------------------------------------------------
/**
  ������� �� ����������
*/
//---
bool PGConnection::IsValid() const
{
	return GetStatus() == ConnectionStatus::Ok;
}

//------------------------------------------------------------------------------
/**
  �������� ������ ����������
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
  ��������� ������
*/
//---
IExecuteResultPtr PGConnection::Execute(const std::string & query)
{
	std::lock_guard guard(m_mutex);
	return executeImpl(query);
}


//------------------------------------------------------------------------------
/**
  ������� ����������
*/
//---
IExecuteResultStatusPtr PGConnection::BeginTransaction()
{
	auto result = Execute("BEGIN;");
	return result ? result->GetCurrentExecuteStatus() : nullptr;
}


//------------------------------------------------------------------------------
/**
  ������� ���������� � ����������� ���������
*/
//---
IExecuteResultStatusPtr PGConnection::CommitTransaction()
{
	auto result = Execute("END;");
	return result ? result->GetCurrentExecuteStatus() : nullptr;
}


//------------------------------------------------------------------------------
/**
  �������� ���������� (��� ���������� ���������)
*/
//---
IExecuteResultStatusPtr PGConnection::RollbackTransaction()
{
	auto result = Execute("ROLLBACK;");
	return result ? result->GetCurrentExecuteStatus() : nullptr;
}


//------------------------------------------------------------------------------
/**
  ��������������� ����� ��� ���������� Execute. �� ��������� �������.
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
  ������� ��������� ����
*/
//---
IFilePtr PGConnection::CreateRemoteFile()
{
	Oid oid = LoCreate();
	return oid == InvalidOid ? nullptr : std::make_shared<PGRemoteFile>(weak_from_this(), oid);
}


//------------------------------------------------------------------------------
/**
  ������� ��������� ����
*/
//---
bool PGConnection::DeleteRemoteFile(const std::string & filename)
{
	auto oid = fileNameToOid(filename);
	if (!oid)
		return false;

	return LoUnlink(*oid) == 1;
}


//------------------------------------------------------------------------------
/**
  �������� ��������� ����
  \return ���� �������� ��� �����, ������� �� ������������� �������� ����������, �� ���������� nullptr.
          � ��������� ������ ������� ���������� ��������� ���������. ��� ���� ���������� ���� �����
          �� ������������ - ��� ����������� �� ����� ��� �������� ������� IFile::Open.
*/
//---
IFilePtr PGConnection::GetRemoteFile(const std::string & filename)
{
	if (!IsValid())
		return nullptr;

	auto oid = fileNameToOid(filename);
	if (!oid)
		return nullptr;

	return std::make_shared<PGRemoteFile>(weak_from_this(), *oid);
}


//------------------------------------------------------------------------------
/**
  ��������� ������ � �����������
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
  ������� ������� �������� ������
*/
//---
Oid PGConnection::LoCreate()
{
	std::lock_guard guard(m_mutex);
	return lo_create(m_conn, InvalidOid);
}


//------------------------------------------------------------------------------
/**
  ������� ������� �������� ������
*/
//---
int PGConnection::LoUnlink(Oid objId)
{
	std::lock_guard guard(m_mutex);
	return lo_unlink(m_conn, objId);
}


//------------------------------------------------------------------------------
/**
  ������� ������� �������� ������
*/
//---
int PGConnection::LoOpen(Oid objId, int mode)
{
	std::lock_guard guard(m_mutex);
	return lo_open(m_conn, objId, mode);
}


//------------------------------------------------------------------------------
/**
  ��������� ������ �� �������� ��������� �������
*/
//---
int PGConnection::LoRead(int fd, char * buffer, size_t len)
{
	std::lock_guard guard(m_mutex);	
	return lo_read(m_conn, fd, buffer, len);
}


//------------------------------------------------------------------------------
/**
  �������� ������ � ������� �������� ������
*/
//---
int PGConnection::LoWrite(int fd, const char * data, size_t len)
{
	std::lock_guard guard(m_mutex);
	return lo_write(m_conn, fd, data, len);
}


//------------------------------------------------------------------------------
/**
  ������������� � ������� �������� �������
*/
//---
pg_int64 PGConnection::LoLseek64(int fd, pg_int64 offset, int whence)
{
	std::lock_guard guard(m_mutex);
	return lo_lseek64(m_conn, fd, offset, whence);
}


//------------------------------------------------------------------------------
/**
  �������� ������� ��������� � ������� �������� �������
*/
//---
pg_int64 PGConnection::LoTell64(int fd)
{
	std::lock_guard guard(m_mutex);
	return lo_tell64(m_conn, fd);
}


//------------------------------------------------------------------------------
/**
  ����� (��� ���������) ������� �������� ������
*/
//---
pg_int64 PGConnection::LoTruncate64(int fd, pg_int64 len)
{
	std::lock_guard guard(m_mutex);
	return lo_truncate64(m_conn, fd, len);
}


//------------------------------------------------------------------------------
/**
  ������� ������� �������� ������
*/
//---
int PGConnection::LoClose(int fd)
{
	std::lock_guard guard(m_mutex);
	return lo_close(m_conn, fd);
}


//------------------------------------------------------------------------------
/**
  ������� �������� ����� � �������� �������������
*/
//---
std::optional<Oid> PGConnection::fileNameToOid(const std::string & filename)
{
	auto number = utils::string::StringToNumber(
		static_cast<long(*)(const std::string&, size_t *, int)>(&std::stol), filename);
	if (!number || *number < std::numeric_limits<Oid>::min() || *number > std::numeric_limits<Oid>::max())
		return std::nullopt;

	return static_cast<Oid>(*number);
}
