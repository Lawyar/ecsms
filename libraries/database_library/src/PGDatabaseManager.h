#pragma once

#include <IDatabaseManager.h>


//------------------------------------------------------------------------------
/**
  ����� ��������� ���� ������
  todo : ��������� ����������� - �������� ���� ���������� (������� �� ������
  ��������� ��������� ����������, ����� �� ������������, � �� ��������� ������ ���
  ������. ����� �� ���� �������� ���������� - ���������� ��������� � ���� �����
  ����������).
*/
//---
class PGDatabaseManager : public IDatabaseManager
{
public:
	/// �������� ����������
	virtual IConnectionPtr GetConnection(const std::string & connectionInfo) override;

	/// �������� ����������� EAV-��������
	virtual IExecutorEAVPtr GetExecutorEAV(const IConnectionPtr &  connection) override;

	/// �������� ��������� SQL-�����
	virtual ISQLTypeConverterPtr GetSQLTypeConverter() const override;
};
