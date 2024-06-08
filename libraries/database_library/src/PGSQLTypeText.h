#pragma once

#include <DataType/ISQLTypeText.h>

//------------------------------------------------------------------------------
/**
  ���������� SQL-���� ������ "text" ��� PostgreSQL
*/
//---
class PGSQLTypeText : public ISQLTypeText
{
	std::optional<std::string> m_value; ///< ��������

public:
	/// �����������
	PGSQLTypeText() = default;
	/// ����������� �� ���������
	PGSQLTypeText(const std::string & value);
	/// ����������� �����������
	PGSQLTypeText(const PGSQLTypeText &) = default;
	/// ����������� �����������
	PGSQLTypeText(PGSQLTypeText &&) = default;
	/// �������� ������������ ������������
	PGSQLTypeText& operator=(const PGSQLTypeText &) = default;
	/// �������� ������������ ������������
	PGSQLTypeText& operator=(PGSQLTypeText &&) = default;

public: // ISQLTypeText
	/// �������� ��������
	virtual std::optional<std::string> GetValue() const override;
	/// ���������� ��������
	virtual void SetValue(const std::string & value) override;


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
