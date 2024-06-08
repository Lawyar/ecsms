#pragma once

#include <IExecuteResultStatus.h>

#include <string>

//------------------------------------------------------------------------------
/**
  ��������� ��� ������������� ������
*/
//---
struct ErrorMessages
{
	static constexpr auto ISQLTypeConverter_GetSQLVariable = "ISQLTypeConverter::GetSQLVariable: Failed to get SQL-variable";
	static constexpr auto IConnection_Execute = "IConnection::Execute: Failed to execute query";
	static constexpr auto IExecuteResult_GetCurrentExecuteStatus = "IExecuteResult::GetCurrentExecuteStatus: Failed to get execute status";
	static constexpr auto ISQLType_ReadFromSQL = "ISQLType::ReadFromSQL: Failed to read data into SQL-variable";
	static constexpr auto ISQLType_GetValue = "ISQLType::GetValue: Failed to get value from SQL-variable";
};


//------------------------------------------------------------------------------
/**
  ����� ��� ������� ���������� �������, ���������������� �� ���������� ������,
  ������������ � ���� ������
  ��� ��� ��������� �������, ������� �� ����� ���� ����������� ������� ��������.
*/
//---
class InternalExecuteResultStatus : public IExecuteResultStatus
{
private:
	/// ��������� ���������� �� ������ (������������� �� ������, ������������ ���� �������)
	struct InternalInfo
	{
		ResultStatus status;      ///< ���������� ������ �������
		std::string errorMessage; ///< ���������� ��������� �� ������
	};

private:
	const InternalInfo m_internalInfo; ///< ���������� ���������� �� ������ �� ������ �������� �������

public:
	/// �����������
	InternalExecuteResultStatus(ResultStatus status, const std::string & errorMessage);

public:
	/// �������� ������ �������
	virtual ResultStatus GetStatus() const override;
	/// �������� ��������� �� ������, ��������� � �������� (���������� ������ ������, ���� ������ �� ���������)
	virtual std::string GetErrorMessage() const override;

public:
	/// �������� ������ ���������� ������
	static IExecuteResultStatusPtr GetInternalError(const std::string & errorMessage = "Unknown error",
		ResultStatus status = ResultStatus::FatalError);
	/// �������� �������� ������
	static IExecuteResultStatusPtr GetSuccessStatus(ResultStatus status = ResultStatus::OkWithData);

};

