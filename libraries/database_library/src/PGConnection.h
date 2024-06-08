#pragma once

#include <IConnection.h>

#include <libpq-fe.h>

#include <mutex>

//------------------------------------------------------------------------------
/**
  ���������� � ����� ������ PostgreSQL.
*/
//---
class PGConnection : public IConnection
{
	PGconn * m_conn = nullptr; ///< ����������
	std::mutex m_mutex; ///< ������� ��� ���������� ������ � ������������� �����
						///< (����� ��������� ������� ���������� ���� ����������)
public:
	/// �����������
	PGConnection(const std::string & connectionInfo);
	/// ����������
	~PGConnection();
	/// ����������� �����������
	PGConnection(const PGConnection &) = delete;
	/// ����������� �����������
	PGConnection(PGConnection &&) = delete;
	/// �������� ������������ ������������
	PGConnection& operator=(const PGConnection &) = delete;
	/// �������� ������������ ������������
	PGConnection& operator=(PGConnection &&) = delete;

public:
	/// ������� �� ����������
	virtual bool IsValid() const override;
	/// �������� ������ ����������
	virtual ConnectionStatus GetStatus() const override;

	/// ��������� ������. ������ ����� ��������� ��������� SQL ������, ����� ��� ����� ��������� � ������ ����� ����������
	/// (������ ���� ������� BEGIN/COMMIT �� �������� ���� � ������, ����� ��������� ��� �� ��������� ����������)
	virtual IExecuteResultPtr Execute(const std::string & query) override;

protected:
	// todo: IConnection::Execute ���������� � ��������� �������

	/// ��������� ������ � �����������. ������ �� ����� ��������� ����� ����� SQL-�������.
	/// \param query ������ � ��������. ��������� � ������� ������������ ������ ������� � ����������� ������� ���������,
	///              ������� � �������. ������ �������: "INSERT INTO table VALUES($1, $2);".
	/// \param args ������ ����������, ������ �� ������� ����� ���� ���� ��������� �������, ���� �������� ����.
	/// \param types ������ ����� ����������.
	///              ���� ������� ������ ������, �� ���� ���������� ����� ��������� �������������.
	///              ����� ����� ����������� ������� �������� ��������� � ��������� �����.
	///              � �������� ���� ��������� ����� ���� ������ ����������� ��� (SQLDataType::Unknown) - � ����
	///              ������ ��� ����� ��������� ����� �������� �������������.
	/// \param resultFormat ������, � ������� ����� ����������� ��������� (�������� ��� ���������)
	virtual IExecuteResultPtr Execute(const std::string & singleCommand,
		const std::vector<ExecuteArgType> & args,
		const ResultFormat resultFormat = ResultFormat::Text,
		const std::vector<SQLDataType> & types = {}) override;
};
