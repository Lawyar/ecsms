#include "PGRemoteFile.h"

#include <libpq/libpq-fs.h>

#include <string>
#include <cassert>

//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGRemoteFile::PGRemoteFile(std::weak_ptr<PGConnection> connection, Oid objId)
	: m_connection(connection)
	, m_objId(objId)
{
}


//------------------------------------------------------------------------------
/**
  ����������
*/
//---
PGRemoteFile::~PGRemoteFile()
{
	Close();
}


//------------------------------------------------------------------------------
/**
  �������� ��� ����� (��� �������� ��������� ������� ��� - ��� �������������)
*/
//---
std::string PGRemoteFile::GetFileName() const
{
	return std::to_string(m_objId);
}


//------------------------------------------------------------------------------
/**
  ��������� FileOpenMode � ����, ������� ��������� libpq
*/
//---
static std::optional<int> FileOpenModeToFlag(FileOpenMode mode)
{
	std::optional<int> flag;
	
	return flag;
}


//------------------------------------------------------------------------------
/**
  ������� ����
*/
//---
bool PGRemoteFile::Open(FileOpenMode openMode)
{
	int flag = 0;
	switch (openMode)
	{
	case FileOpenMode::Read:
		flag = INV_READ;
		break;
	case FileOpenMode::Write:
		flag = INV_WRITE;
		break;
	case FileOpenMode::Append:
		flag = INV_WRITE;
		break;
	default:
		// ����������� ���
		assert(false);
		return false;
	}

	auto connection = m_connection.lock();
	if (!connection || !connection->IsValid())
		return false;

	int fd = connection->LoOpen(m_objId, flag);
	if (fd == -1)
		return false;

	bool errorOccured = false; // ��������� ������
	if (openMode == FileOpenMode::Write)
	{
		// ���� ���� ����������� �� ������, ������� ��� ����������
		errorOccured = (connection->LoTruncate64(fd, 0) == -1);
	}
	else if (openMode == FileOpenMode::Append)
	{
		// ���� ���� ����������� �� ��������, ���������� ������ � ����� �����
		errorOccured = (connection->LoLseek64(fd, 0, SEEK_END) == -1);
	}

	if (errorOccured)
	{
		// ���� ��������� ������, ������� ����
		connection->LoClose(fd);
		return false;
	}

	m_fd = fd;
	m_openMode = openMode;
	return true;
}


//------------------------------------------------------------------------------
/**
  ������� ����
*/
//---
bool PGRemoteFile::Close()
{
	if (!m_fd)
		// ������ ���������
		return false;

	auto connection = m_connection.lock();
	if (!connection || !connection->IsValid())
		return false;

	if (connection->LoClose(*m_fd) != 0)
		return false;

	m_fd = std::nullopt;
	m_openMode = std::nullopt;
	return true;
}


//------------------------------------------------------------------------------
/**
  ��������� �����
*/
//---
bool PGRemoteFile::ReadBytes(size_t count, std::vector<char> & buffer)
{
	if (!m_fd || !m_openMode)
		// ���� �� ������
		return false;

	if (*m_openMode != FileOpenMode::Read)
		// ���� �� ������ �� ������
		return false;

	auto connection = m_connection.lock();
	if (!connection)
		return false;

	// ���������� ����, �������� �� ��� (16 ��)
	static constexpr const size_t c_maxPackageSize = 16'000'000ULL;

	bool result = true;

	std::vector<char> data(count);
	// ���������� ���������� ������
	size_t readBytesCount = 0;
	for (size_t currentPos = 0; currentPos < data.size(); currentPos += c_maxPackageSize)
	{
		// ������ �������� ��������� ������
		size_t currentPackageSize = c_maxPackageSize <= data.size() - currentPos
			? c_maxPackageSize : data.size() - currentPos;

		int readBytesCountInCurrentPackage = connection->LoRead(*m_fd, &data[currentPos],
			currentPackageSize);
		if (readBytesCountInCurrentPackage < 0)
		{
			// ��������� ������
			result = false;
			break;
		}

		readBytesCount += static_cast<size_t>(readBytesCountInCurrentPackage);
	}

	data.resize(static_cast<size_t>(readBytesCount));
	buffer.insert(buffer.end(), std::make_move_iterator(data.begin()), std::make_move_iterator(data.end()));
	
	return result;
}


//------------------------------------------------------------------------------
/**
  �������� �����
*/
//---
bool PGRemoteFile::WriteBytes(const std::vector<char> & data, size_t * numberOfBytesWritten)
{
	if (numberOfBytesWritten)
		*numberOfBytesWritten = 0;

	if (!m_fd || !m_openMode)
		// ���� �� ������
		return false;

	if (*m_openMode != FileOpenMode::Write && *m_openMode != FileOpenMode::Append)
		// ���� �� ������ �� ������/��������
		return false;

	auto connection = m_connection.lock();
	if (!connection || !connection->IsValid())
		return false;

	// ���������� ����, ������������ �� ��� (16 ��)
	static constexpr const size_t c_maxPackageSize = 16'000'000ULL;

	bool result = true;

	// ���������� ���������� ������
	size_t writtenBytesCount = 0;
	for (size_t currentPos = 0; currentPos < data.size(); currentPos += c_maxPackageSize)
	{
		// ������ �������� ������������� ������
		size_t currentPackageSize = c_maxPackageSize <= data.size() - currentPos
			? c_maxPackageSize : data.size() - currentPos;

		int writtenBytesCountInCurrentPackage = connection->LoWrite(*m_fd, &data[currentPos],
			currentPackageSize);
		if (writtenBytesCount < 0)
		{
			// ��������� ������
			result = false;
			break;
		}

		writtenBytesCount += static_cast<size_t>(writtenBytesCountInCurrentPackage);
	}

	if (numberOfBytesWritten)
		*numberOfBytesWritten = writtenBytesCount;

	return result;
}
