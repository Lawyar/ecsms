#pragma once

#include <IConnection.h>

#include <libpq-fe.h>

#include <mutex>

class PGConnection;
/// ����������� ��������� ��������� �� PGConnection
using PGConnectionSPtr = std::shared_ptr<PGConnection>;
/// ����������� ����������� ��������� �� PGConnection
using PGConnectionWPtr = std::weak_ptr<PGConnection>;

//------------------------------------------------------------------------------
/**
  ���������� � ����� ������ PostgreSQL.
*/
//---
class PGConnection : public std::enable_shared_from_this<PGConnection>, public IConnection
{
	PGconn * m_conn = nullptr; ///< ����������
	std::mutex m_mutex; ///< ������� ��� ���������� ������ � ������������� �����
						///< (����� ��������� ������� ���������� ���� ����������)

public:
	/// ����������
	virtual ~PGConnection() override;

	/// ������� ���������
	static PGConnectionSPtr Create(const std::string & connectionInfo);

private:
	/// �����������
	PGConnection(const std::string & connectionInfo);
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

	/// ��������� ������.
	/// ������ ����� ��������� ��������� SQL ������, ����� ��� ������������� ����� ���������
	/// � ������ ����� ����������, ����� �������, ����� ������� ��� ��������� �� ����������
	/// �� ������������ ���� (����������� ���������� ������ BEGIN/COMMIT � ����� ������� query ���
	/// ����������� ������ ������� BeginTransaction/CommitTransaction).
	virtual IExecuteResultPtr Execute(const std::string & query) override;

	/// ������� ����������
	virtual IExecuteResultStatusPtr BeginTransaction() override;
	/// ������� ���������� � ����������� ���������
	virtual IExecuteResultStatusPtr CommitTransaction() override;
	/// �������� ���������� (��� ���������� ���������)
	virtual IExecuteResultStatusPtr RollbackTransaction() override;


private:
	/// ��������������� ����� ��� ���������� Execute. �� ��������� �������.
	IExecuteResultPtr executeImpl(const std::string & query);

public:
	/// ������� ��������� ����.
	/// ����� ����� ���������� ��� ������ ����������, ��� � �� � ���.
	/// ��� ������ ������ ���������� ��������� ������� ������� ����������:
	/// ���� ���������� �� ���� ��������� ��������� ��� ���� ��������� �������, �� ���� �� ��������.
	/// ���������������: ����������� ������ � ��������� ������ �������� ������ � ������ ����������.
	virtual IFilePtr CreateRemoteFile() override;
	/// ������� ����.
	/// ����� ����� ���������� ��� � ������ ����������, ��� � �� � ���.
	/// ��� ������ ������ ���������� ��������� ������� ������� ����������:
	/// ���� ���������� �� ���� ��������� ��������� ��� ���� ��������� �������, �� ���� �� ��������.
	virtual bool DeleteRemoteFile(const std::string & filename) override;
	/// �������� ����.
	/// ���������������: ����������� ������ � ��������� ������ �������� ������ � ������ ����������.
	/// \return ���� �������� ��� �����, ������� �� ������������� �������� ����������, �� ���������� nullptr.
	///         � ��������� ������ ������� ���������� ��������� ���������. ��� ���� ���������� ���� �����
	///         �� ������������ - ��� ����������� �� ����� ��� �������� ������� IFile::Open.
	virtual IFilePtr GetRemoteFile(const std::string & filename) override;

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

public: // ������ ��� ������ � �������� ��������� ���������
		// todo: �������� �����������, ��������� �������������� �������� (������������ �������� PQerrorMessage)

	/// ������� ������� �������� ������
	Oid LoCreate();
	/// ������� ������� �������� ������
	int LoUnlink(Oid objId);
	/// ������� ������� �������� ������
	int LoOpen(Oid objId, int mode);
	/// ��������� ������ �� �������� ��������� �������
	int LoRead(int fd, char * buffer, size_t len);
	/// �������� ������ � ������� �������� ������
	int LoWrite(int fd, const char * data, size_t len);
	/// ������������� � ������� �������� �������
	pg_int64 LoLseek64(int fd, pg_int64 offset, int whence);
	/// �������� ������� ��������� � ������� �������� �������
	pg_int64 LoTell64(int fd);
	/// ����� (��� ���������) ������� �������� ������
	pg_int64 LoTruncate64(int fd, pg_int64 len);
	/// ������� ������� �������� ������
	int LoClose(int fd);

private:
	/// ������� �������� ����� � �������� �������������
	std::optional<Oid> fileNameToOid(const std::string & filename);
};
