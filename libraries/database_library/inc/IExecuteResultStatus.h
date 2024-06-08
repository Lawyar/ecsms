#pragma once

#include <memory>
#include <string>

//------------------------------------------------------------------------------
/**
  ������ ���������� �������
*/
//---
enum class ResultStatus
{
	OkWithoutData, ///< �������� ���������� �������, �� ������������ ������� ������
	OkWithData,    ///< �������� ���������� �������, ������������ ������
	InProgress,    ///< ������� �����������
	NonFatalError, ///< ��������� �� ��������� ������ (����������� ��� ��������������)
	FatalError,    ///< ��������� ��������� ������
	EmptyQuery,    ///< ������ �������, ������������ �������, ���� ������
	Unknown        ///< ����������� ������
};


//------------------------------------------------------------------------------
/**
  ��������� ������� ���������� ������� � ���� ������.
*/
//---
class IExecuteResultStatus
{
public:
	/// ����������
	virtual ~IExecuteResultStatus() = default;

public:
	/// �������� ������ �������
	virtual ResultStatus GetStatus() const = 0;
	/// �������� ��������� �� ������, ��������� � �������� (���������� ������ ������, ���� ������ �� ���������)
	virtual std::string GetErrorMessage() const = 0;
	/// ����� �������� ������ ��� ��������������
	bool HasError() const
	{
		ResultStatus status = GetStatus();
		return status == ResultStatus::EmptyQuery || status == ResultStatus::NonFatalError || status == ResultStatus::FatalError || status == ResultStatus::Unknown;
	}
};

/// ��������� �� IExecuteResultStatus
using IExecuteResultStatusPtr = std::shared_ptr<IExecuteResultStatus>;
