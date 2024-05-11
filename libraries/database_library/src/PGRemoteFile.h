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
	virtual bool Open(const std::vector<FileOpenMode> & openModes) override;
	/// Закрыть файл
	virtual bool Close() override;
	/// Попытаться прочесть байты
	virtual std::optional<std::vector<char>> ReadBytes(size_t count) override;
	/// Попытаться записать байты
	virtual bool WriteBytes(const std::vector<char> & data) override;
};
