#pragma once

#include <IFile.h>
#include <PGConnection.h>

#include <libpq-fe.h>


//------------------------------------------------------------------------------
/**
  Класс для взаимодействия с большим бинарным объектом на сервере PostgreSQL
*/
//---
class PGRemoteFile : public IFile
{
	std::weak_ptr<PGConnection> m_connection; ///< Соединение
	Oid m_objId; ///< Идентификатор большого бинарного объекта
	std::optional<int> m_fd; ///< Дескриптор открытого большого бинарного объекта
	std::optional<FileOpenMode> m_openMode; ///< Режим открытия файла (если std::nullopt, то файл не открыт)

public:
	/// Конструктор
	PGRemoteFile(std::weak_ptr<PGConnection> connection, Oid objId);
	/// Деструктор
	virtual ~PGRemoteFile() override;

	/// Конструктор копирования
	PGRemoteFile(const PGRemoteFile &) = delete;
	/// Конструктор перемещения
	PGRemoteFile(PGRemoteFile &) = delete;
	/// Оператор присваивания копированием
	PGRemoteFile& operator=(const PGRemoteFile &) = delete;
	/// Оператор присваивания перемещением
	PGRemoteFile& operator=(PGRemoteFile &&) = delete;

public:
	/// Получить имя файла (для большого бинарного объекта имя - это идентификатор)
	virtual std::string GetFileName() const override;
	/// Открыть файл
	/// \param openMode Режим открытия
	/// \return Статус выполнения операции
	virtual bool Open(FileOpenMode openMode) override;
	/// Закрыть файл
	/// Файл, оставшийся открытым в конце транзакции, будет закрыт автоматически
	/// \return Статус выполнения операции
	virtual bool Close() override;
	/// Попытаться прочесть байты
	/// \param count Количество байт, которое требуется попытаться прочесть.
	/// \param buffer Буфер, в который требуется прочитать байты.
	///   Результат будет дописан в конец этого буфера.
	///   Если в буфер было записано менее, чем count байтов, это может свидельствовать о том,
	///   что файл закончился, или о том, что произошла ошибка.
	/// \return Статус выполнения операции.
	virtual bool ReadBytes(size_t count, std::vector<char> & buffer) override;
	/// Попытаться записать байты
	/// \param data Массив байтов, который требуется записать.
	/// \param numberOfBytesWritten Количество успешно записанных байтов.
	/// \return Статус выполнения операции.
	virtual bool WriteBytes(const std::vector<char> & data, size_t * numberOfBytesWritten) override;
};
