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
	PGSQLTypeText(std::string && value);
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
	virtual const std::optional<std::string> & GetValue() const override;
	/// ���������� ��������
	/// \param value ������, �� ������� ����� ���������� ��������.
	///              ��� ������ ������ ����� �������, ����� ��������� ����������.
	virtual void SetValue(std::string && value) override;


public: // ISQLType
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

private:
	/// ��� ������������ ������
	static bool isInvalidChar(char c);
};
