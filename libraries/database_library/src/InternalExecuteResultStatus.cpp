#include "InternalExecuteResultStatus.h"

//------------------------------------------------------------------------------
/**
  �����������
*/
//---
InternalExecuteResultStatus::InternalExecuteResultStatus(ResultStatus status,
	const std::string & errorMessage)
	: m_internalInfo{ status, errorMessage }
{
}


//------------------------------------------------------------------------------
/**
  �������� ������
*/
//---
ResultStatus InternalExecuteResultStatus::GetStatus() const
{
	return m_internalInfo.status;
}


//------------------------------------------------------------------------------
/**
  �������� ��������� �� ������
*/
//---
std::string InternalExecuteResultStatus::GetErrorMessage() const
{
	return m_internalInfo.errorMessage;
}


//------------------------------------------------------------------------------
/**
  �������� ������ ���������� ������
*/
//---
IExecuteResultStatusPtr InternalExecuteResultStatus::GetInternalError(const std::string & errorMessage, ResultStatus status)
{
	return std::make_shared<InternalExecuteResultStatus>(status, errorMessage);
}


//------------------------------------------------------------------------------
/**
  �������� �������� ������
*/
//---
IExecuteResultStatusPtr InternalExecuteResultStatus::GetSuccessStatus(ResultStatus status)
{
	return std::make_shared<InternalExecuteResultStatus>(status, "");
}
