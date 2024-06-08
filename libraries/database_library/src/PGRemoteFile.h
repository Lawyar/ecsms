#pragma once

#include <IFile.h>
#include <PGConnection.h>

#include <libpq-fe.h>


//------------------------------------------------------------------------------
/**
  ����� ��� �������������� � ������� �������� �������� �� ������� PostgreSQL
*/
//---
class PGRemoteFile : public IFile
{
	std::weak_ptr<PGConnection> m_connection; ///< ����������
	Oid m_objId; ///< ������������� �������� ��������� �������
	std::optional<int> m_fd; ///< ���������� ��������� �������� ��������� �������
	std::optional<FileOpenMode> m_openMode; ///< ����� �������� ����� (���� std::nullopt, �� ���� �� ������)

public:
	/// �����������
	PGRemoteFile(std::weak_ptr<PGConnection> connection, Oid objId);
	/// ����������
	virtual ~PGRemoteFile() override;

	/// ����������� �����������
	PGRemoteFile(const PGRemoteFile &) = delete;
	/// ����������� �����������
	PGRemoteFile(PGRemoteFile &) = delete;
	/// �������� ������������ ������������
	PGRemoteFile& operator=(const PGRemoteFile &) = delete;
	/// �������� ������������ ������������
	PGRemoteFile& operator=(PGRemoteFile &&) = delete;

public:
	/// �������� ��� ����� (��� �������� ��������� ������� ��� - ��� �������������)
	virtual std::string GetFileName() const override;
	/// ������� ����
	/// \param openMode ����� ��������
	/// \return ������ ���������� ��������
	virtual bool Open(FileOpenMode openMode) override;
	/// ������� ����
	/// ����, ���������� �������� � ����� ����������, ����� ������ �������������
	/// \return ������ ���������� ��������
	virtual bool Close() override;
	/// ���������� �������� �����
	/// \param count ���������� ����, ������� ��������� ���������� ��������.
	/// \param buffer �����, � ������� ��������� ��������� �����.
	///   ��������� ����� ������� � ����� ����� ������.
	///   ���� � ����� ���� �������� �����, ��� count ������, ��� ����� ��������������� � ���,
	///   ��� ���� ����������, ��� � ���, ��� ��������� ������.
	/// \return ������ ���������� ��������.
	virtual bool ReadBytes(size_t count, std::vector<char> & buffer) override;
	/// ���������� �������� �����
	/// \param data ������ ������, ������� ��������� ��������.
	/// \param numberOfBytesWritten ���������� ������� ���������� ������.
	/// \return ������ ���������� ��������.
	virtual bool WriteBytes(const std::vector<char> & data, size_t * numberOfBytesWritten) override;
};
