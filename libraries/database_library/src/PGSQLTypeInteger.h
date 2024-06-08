#pragma once

#include <DataType/ISQLTypeInteger.h>

//------------------------------------------------------------------------------
/**
  ���������� SQL-���� ������ "integer" ��� PostgreSQL
*/
//---
class PGSQLTypeInteger : public ISQLTypeInteger
{
	std::optional<int> m_value; ///< ��������

public:
	/// �����������
	PGSQLTypeInteger() = default;
	/// ����������� �� ���������
	PGSQLTypeInteger(int value);
	/// ����������� �����������
	PGSQLTypeInteger(const PGSQLTypeInteger &) = default;
	/// ����������� �����������
	PGSQLTypeInteger(PGSQLTypeInteger &&) = default;
	/// �������� ������������ ������������
	PGSQLTypeInteger& operator=(const PGSQLTypeInteger &) = default;
	/// �������� ������������ ������������
	PGSQLTypeInteger& operator=(PGSQLTypeInteger &&) = default;

public: // ISQLTypeInteger
	/// �������� ��������
	virtual std::optional<int> GetValue() const override;
	/// ���������� ��������
	virtual void SetValue(int value) override;

public: // ISQLType
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
