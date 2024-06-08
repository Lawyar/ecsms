#pragma once

#include <DataType/ISQLTypeByteArray.h>

#include <vector>

//------------------------------------------------------------------------------
/**
  ���������� SQL-���� ������ "bytea" ��� PostgreSQL
*/
//---
class PGSQLTypeByteArray : public ISQLTypeByteArray
{
	std::optional<std::vector<char>> m_value; ///< ��������

public:
	/// �����������
	PGSQLTypeByteArray() = default;
	/// ����������� �� ���������
	PGSQLTypeByteArray(std::vector<char> && value);
	/// ����������� �����������
	PGSQLTypeByteArray(const PGSQLTypeByteArray &) = default;
	/// ����������� �����������
	PGSQLTypeByteArray(PGSQLTypeByteArray &&) = default;
	/// �������� ������������ ������������
	PGSQLTypeByteArray& operator=(const PGSQLTypeByteArray &) = default;
	/// �������� ������������ ������������
	PGSQLTypeByteArray& operator=(PGSQLTypeByteArray &&) = default;

public:
	/// �������� ��������
	virtual const std::optional<std::vector<char>> & GetValue() const override;
	/// ���������� ��������
	/// \param value ������, �� �������� ����� ���������� ��������.
	///              ��� ������ ������ ����� ������, ����� ��������� ����������.
	virtual void SetValue(std::vector<char> && value) override;

public:
	/// ��������������� � ������
	virtual std::optional<std::string> ToSQLString() const override;
	/// �������� �������� SQL-����
	virtual const std::string & GetTypeName() const override;

public:
	/// ��������� �������� �� ������
	/// \param value ������, �� ������� ����� ������ ��������.
	///              ���� ������ ������ �������, �� ������ ����� �������, ����� ��������� ����������.
	/// \return ������� �� ����������� ��������
	virtual bool ReadFromSQL(std::string && value) override;

protected:
	/// ��������� �������� �� ������� ����
	/// \param value ������, �� �������� ����� ������ ��������.
	///              ���� ������ ������ �������, �� ������ ����� ������, ����� ��������� ����������.
	/// \return ������� �� ����������� ��������
	virtual bool ReadFromSQL(std::vector<char> && value) override;
};
