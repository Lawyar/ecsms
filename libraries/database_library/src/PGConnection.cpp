#include "PGConnection.h"

#include <PGSQLDataTypeUtility.h>
#include <PGExecuteResult.h>


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
	PGresult * pqResult = PQexec(m_conn, query.c_str());
	IExecuteResultPtr result(new PGExecuteResult(pqResult));
	return result;
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
