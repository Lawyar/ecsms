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
	PGSQLTypeByteArray(const std::vector<char> & value);
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
	virtual std::optional<std::vector<char>> GetValue() const override;
	/// ���������� ��������
	virtual void SetValue(const std::vector<char> & value) override;

public:
	/// ��������������� � ������
	virtual std::optional<std::string> ToString() const override;
	/// �������� �������� SQL-����
	virtual const std::string & GetTypeName() const override;

public:
	/// ��������� �������� �� ������
	/// \return ������� �� ����������� ��������
	virtual bool ReadFrom(const std::string & value) override;
	/// ��������� �������� �� ������� ����
	/// \return ������� �� ����������� ��������
	virtual bool ReadFrom(const std::vector<char> & value) override;
};
