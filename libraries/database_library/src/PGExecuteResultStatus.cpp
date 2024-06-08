#include "PGExecuteResultStatus.h"


//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGExecuteResultStatus::PGExecuteResultStatus(const PGresult * pgResult)
	: m_externalInfo{ PQresultStatus(pgResult), PQresultErrorMessage(pgResult) }
	, m_internalInfo(createInternalInfo(m_externalInfo.status))
{	
}


//------------------------------------------------------------------------------
/**
  �������� ������
*/
//---
ResultStatus PGExecuteResultStatus::GetStatus() const
{
	return m_internalInfo.status;
}


//------------------------------------------------------------------------------
/**
  �������� ��������� �� ������, ��������� � ��������
  \return ��������� �� ������, ��������� � ��������,
		  ��� ������ ������, ���� ������ �� ���������.
*/
//---
std::string PGExecuteResultStatus::GetErrorMessage() const
{
	std::string error;
	if (!m_internalInfo.errorMessage.empty())
		error += "[Internal Error] " + m_internalInfo.errorMessage;
	if (!m_externalInfo.errorMessage.empty())
		error += std::string(!error.empty() ? "; " : "") + "[PostgreSQL Error] " + m_externalInfo.errorMessage;
	return error;
}


//------------------------------------------------------------------------------
/**
  ������� ���������� ������������� ������
*/
//---
PGExecuteResultStatus::InternalInfo PGExecuteResultStatus::createInternalInfo(ExecStatusType pqStatus)
{
	InternalInfo internalInfo{ResultStatus::Unknown, std::string()};
	switch (pqStatus)
	{
	case PGRES_EMPTY_QUERY: // ������, ������������ �������, ���� ������
		internalInfo.status = ResultStatus::EmptyQuery;
		break;
	case PGRES_COMMAND_OK: // �������� ���������� �������, �� ������������ ������� ������
		internalInfo.status = ResultStatus::OkWithoutData;
		break;
	case PGRES_TUPLES_OK: // �������� ���������� �������, ������������ ������ (�����, ��� SELECT ��� SHOW)
		internalInfo.status = ResultStatus::OkWithData;
		break;
	case PGRES_COPY_OUT: // ����� ������� ������ Copy Out (� �������)
	case PGRES_COPY_IN: // ����� ������� ������ Copy In (�� ������)
	case PGRES_COPY_BOTH: // ����� ������� ������ Copy In/Out (�� ������ � � �������)
		internalInfo.status = ResultStatus::InProgress;
		break;
	case PGRES_NONFATAL_ERROR: // ��������� �� ��������� ������ (����������� ��� ��������������)
		internalInfo.status = ResultStatus::NonFatalError;
		break;
	case PGRES_FATAL_ERROR: // ��������� ��������� ������
	case PGRES_BAD_RESPONSE: // ����� ������� �� ��� ���������
		internalInfo.status = ResultStatus::FatalError;
		break;
	case PGRES_SINGLE_TUPLE:
		// ��������� PGresult �������� ������ ���� �������������� ������, ������������ ������� ��������.
		// ���� ������ ����� ����� ������ �����, ����� ��� ������� ������� ��� ������ ����� �����������
		// ������.
		// ������ ���� ����� �� ����������, ������� ���� ������ �� ������ ������������.
		internalInfo.errorMessage = "[Error] The result status corresponds to the line-by-line output mode, "
			"but the use of the line-by-line output mode is not implemented.";
		internalInfo.status = ResultStatus::FatalError;
		break;
	default:
		internalInfo.errorMessage = "[Error] Unknown request result status.";
		internalInfo.status = ResultStatus::Unknown;
		break;
	}

	return internalInfo;
}
