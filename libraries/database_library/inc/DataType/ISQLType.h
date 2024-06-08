#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

#include <DataType/SQLDataType.h>

//------------------------------------------------------------------------------
/**
  ��������� SQL-���� ������
*/
//---
class ISQLType
{
public:
	/// ����������
	virtual ~ISQLType() = default;

public:
	/// ��������������� � ������
	virtual std::optional<std::string> ToSQLString() const = 0;
	/// �������� �������� SQL-����
	virtual const std::string & GetTypeName() const = 0;

	/// ���������� �������� ������
	bool IsEmpty() const { return !ToSQLString().has_value(); }

public:
	/// �������� ��� ������
	virtual SQLDataType GetType() const = 0;

public:
	/// ��������� �������� �� ������, ���������� �� ���������� SQL-�������
	/// \param value ������, �� ������� ����� ������ ��������.
	///              ���� ������ ������ �������, �� ������ ����� �������, ����� ��������� ����������.
	/// \return ������� �� ����������� ��������
	virtual bool ReadFromSQL(std::string && value) = 0;

protected:
	// todo: IConnection::Execute ���������� � ��������� �������

	/// ��������� �������� �� ������� ����, ����������� �� ���������� SQL-�������
	/// \param value ������, �� �������� ����� ������ ��������.
	///              ���� ������ ������ �������, �� ������ ����� ������, ����� ��������� ����������.
	/// \return ������� �� ����������� ��������
	virtual bool ReadFromSQL(std::vector<char> && value) { return false; }
};


/// ��� ��������� �� ISQLType
using ISQLTypePtr = std::shared_ptr<ISQLType>;
