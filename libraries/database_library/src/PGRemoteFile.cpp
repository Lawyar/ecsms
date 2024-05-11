#include "PGRemoteFile.h"

#include <libpq/libpq-fs.h>

#include <string>
#include <cassert>

//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGRemoteFile::PGRemoteFile(std::weak_ptr<PGConnection> connection, Oid objId)
	: m_connection(connection)
	, m_objId(objId)
{
}


//------------------------------------------------------------------------------
/**
  Деструктор
*/
//---
PGRemoteFile::~PGRemoteFile()
{
	Close();
}


//------------------------------------------------------------------------------
/**
  Получить имя файла (для большого бинарного объекта имя - это идентификатор)
*/
//---
std::string PGRemoteFile::GetFileName() const
{
	return std::to_string(m_objId);
}


//------------------------------------------------------------------------------
/**
  Перевести FileOpenMode в флаг, который принимает libpq
*/
//---
static std::optional<int> FileOpenModeToFlag(FileOpenMode mode)
{
	std::optional<int> flag;
	switch (mode)
	{
	case FileOpenMode::Read:
		flag = INV_READ;
		break;
	case FileOpenMode::Write:
		flag = INV_WRITE;
		break;
	default:
		// Неизвестный тип
		assert(false);
		break;
	}
	return flag;
}


//------------------------------------------------------------------------------
/**
  Перевести массив FileOpenMode в набор флагов, который принимает libpq
*/
//---
static std::optional<int> FileOpenModesToFlags(const std::vector<FileOpenMode> modes)
{
	if (modes.empty())
		return std::nullopt;

	int flags = 0;
	for (auto && mode : modes)
	{
		std::optional<int> flag = FileOpenModeToFlag(mode);
		if (!flag)
			return std::nullopt;

		flags |= *flag;
	}

	return flags;
}


//------------------------------------------------------------------------------
/**
  Открыть файл
*/
//---
bool PGRemoteFile::Open(const std::vector<FileOpenMode> & openModes)
{
	std::optional<int> flags = FileOpenModesToFlags(openModes);
	if (!flags)
		return false;

	auto connection = m_connection.lock();
	if (!connection)
		return false;

	int fd = connection->LoOpen(m_objId, *flags);
	if (fd == -1)
		return false;

	m_fd = fd;
	return true;
}


//------------------------------------------------------------------------------
/**
  Закрыть файл
*/
//---
bool PGRemoteFile::Close()
{
	if (!m_fd)
		// Нечего закрывать
		return false;

	auto connection = m_connection.lock();
	if (!connection)
		return false;

	int res = connection->LoClose(*m_fd);
	return res == 0;
}


//------------------------------------------------------------------------------
/**
  Прочитать байты
*/
//---
std::optional<std::vector<char>> PGRemoteFile::ReadBytes(size_t count)
{
	if (!m_fd)
		return std::nullopt;

	auto connection = m_connection.lock();
	if (!connection)
		return std::nullopt;

	std::vector<char> result(count);
	int readBytesCount = connection->LoRead(*m_fd, result.data(), count);
	if (readBytesCount < 0)
		return std::nullopt;

	result.resize(static_cast<size_t>(readBytesCount));
	return result;
}


//------------------------------------------------------------------------------
/**
  Записать байты
*/
//---
bool PGRemoteFile::WriteBytes(const std::vector<char> & data)
{
	if (!m_fd)
		return false;

	auto connection = m_connection.lock();
	if (!connection)
		return false;

	int writtenBytesCount = connection->LoWrite(*m_fd, data.data(), data.size());
	if (writtenBytesCount < 0)
		return false;

	return true;
}
