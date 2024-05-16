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
	
	return flag;
}


//------------------------------------------------------------------------------
/**
  Открыть файл
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
		// Неизвестный тип
		assert(false);
		return false;
	}

	auto connection = m_connection.lock();
	if (!connection)
		return false;

	int fd = connection->LoOpen(m_objId, flag);
	if (fd == -1)
		return false;

	bool errorOccured = false; // случилась ошибка
	if (openMode == FileOpenMode::Write)
	{
		// Если файл открывается на запись, очистим его содержимое
		errorOccured = (connection->LoTruncate64(fd, 0) == -1);
	}
	else if (openMode == FileOpenMode::Append)
	{
		// Если файл открывается на дозапись, переместим курсор в конец файла
		errorOccured = (connection->LoLseek64(fd, 0, SEEK_END) == -1);
	}

	if (errorOccured)
	{
		// Если случилась ошибка, закроем файл
		connection->LoClose(fd);
		return false;
	}

	m_fd = fd;
	m_openMode = openMode;
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

	if (connection->LoClose(*m_fd) != 0)
		return false;

	m_fd = std::nullopt;
	m_openMode = std::nullopt;
	return true;
}


//------------------------------------------------------------------------------
/**
  Прочитать байты
*/
//---
std::optional<std::vector<char>> PGRemoteFile::ReadBytes(size_t count)
{
	if (!m_fd || !m_openMode)
		// Файл не открыт
		return std::nullopt;

	if (*m_openMode != FileOpenMode::Read)
		// Файл не открыт на чтение
		return std::nullopt;

	auto connection = m_connection.lock();
	if (!connection)
		return std::nullopt;

	// todo: Чтение порциями
	static_assert(false);
	
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
std::optional<size_t> PGRemoteFile::WriteBytes(const std::vector<char> & data)
{
	if (!m_fd || !m_openMode)
		// Файл не открыт
		return std::nullopt;

	if (*m_openMode != FileOpenMode::Write && *m_openMode != FileOpenMode::Append)
		// Файл не открыт на запись/дозапись
		return std::nullopt;

	auto connection = m_connection.lock();
	if (!connection)
		return std::nullopt;

	// Количество байт, отправляемых за раз (256 МБ)
	static constexpr const size_t c_maxPackageSize = 256'000'000ULL;

	// Количество записанных байтов
	size_t writtenBytesCount = 0;
	for (size_t currentPos = 0; currentPos < data.size(); currentPos += c_maxPackageSize)
	{
		// Размер текущего отправляемого пакета
		size_t currentPackageSize = currentPos + c_maxPackageSize <= data.size()
			? c_maxPackageSize : data.size() - currentPos;

		int writtenBytesCountInCurrentPackage = connection->LoWrite(*m_fd, &data[currentPos],
			currentPackageSize);
		if (writtenBytesCount < 0)
			// Произошла ошибка, вернем количество успешно записанных байтов
			return writtenBytesCount;

		writtenBytesCount += static_cast<size_t>(writtenBytesCountInCurrentPackage);
	}

	return writtenBytesCount;
}
