#pragma once

#include <IExecuteResultStatus.h>

#include <string>

#include <libpq-fe.h>

//------------------------------------------------------------------------------
/**
  ����� ��� ������� ���������� ������� � ���� ������ PostgreSQL.
*/
//---
class PGExecuteResultStatus : public IExecuteResultStatus
{
private:
	/// ��������� ���������� �� ������ (������������� �� ������, ������������ ���� �������)
	struct InternalInfo
	{
		ResultStatus status;      ///< ���������� ������ �������
		std::string errorMessage; ///< ���������� ��������� �� ������
	};

	/// ������� ���������� �� ������ (������������� �� ������, ������������ libpq)
	struct ExternalInfo
	{
		ExecStatusType status;          ///< ������ ������� PostgreSQL
		std::string errorMessage;       ///< ������ � �������
	};

private:
	const ExternalInfo m_externalInfo; ///< ������� ���������� �� ������ �� ������ �������� �������
	const InternalInfo m_internalInfo; ///< ���������� ���������� �� ������ �� ������ �������� �������
	                                   ///  (������ ���� ���� m_externalInfo, ��������� ��� ������������� ���������� ��� ����)
	
public:
	/// �����������
	PGExecuteResultStatus(const PGresult * pgResult);

public:
	/// �������� ������ �������
	virtual ResultStatus GetStatus() const override;
	/// �������� ��������� �� ������, ��������� � �������� (���������� ������ ������, ���� ������ �� ���������)
	virtual std::string GetErrorMessage() const override;

private:
	/// ������� ���������� ������������� ������
	static PGExecuteResultStatus::InternalInfo createInternalInfo(ExecStatusType pqStatus);
};
