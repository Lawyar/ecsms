#pragma once

#include <memory>
#include <vector>
#include <optional>

//------------------------------------------------------------------------------
/**
  Режим открытия файла
*/
//---
enum class FileOpenMode
{
	Read,  ///< Открыть файл на чтение
	Write, ///< Открыть файл на запись
};

//------------------------------------------------------------------------------
/**
  Интерфейс файла
*/
//---
class IFile
{
public:
	virtual ~IFile() = default;

public:
	/// Получить имя файла
	virtual std::string GetFileName() const = 0;
	/// Открыть файл
	virtual bool Open(const std::vector<FileOpenMode> & openModes = {FileOpenMode::Read,
		FileOpenMode::Write}) = 0;
	/// Закрыть файл
	virtual bool Close() = 0;
	/// Попытаться прочесть байты
	virtual std::optional<std::vector<char>> ReadBytes(size_t count) = 0;
	/// Попытаться записать байты
	virtual bool WriteBytes(const std::vector<char> & data) = 0;
};

/// Указатель на IFile
using IFilePtr = std::shared_ptr<IFile>;
