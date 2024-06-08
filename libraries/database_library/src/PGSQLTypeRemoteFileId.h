#pragma once

#include <DataType/ISQLTypeRemoteFileId.h>

//------------------------------------------------------------------------------
/**
  ���������� SQL-����������, �������������� ������������� ���������� ����� �� �������
*/
//---
class PGSQLTypeRemoteFileId: public ISQLTypeRemoteFileId
{
	std::optional<std::string> m_id; ///< ������������� ���������� �����

public:
	/// ����������� �� ���������
	PGSQLTypeRemoteFileId() = default;
	/// ����������� �� ���������
	PGSQLTypeRemoteFileId(const std::string & id);

public: // ISQLTypeRemoteFileId
	/// �������� �������������
	virtual const std::optional<std::string> & GetValue() const override;

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

private:
	/// ������ ������� ��� ��������� � ������
	static bool isValid(const std::string & str);
};
