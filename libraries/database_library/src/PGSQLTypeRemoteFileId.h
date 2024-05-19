#pragma once

#include <DataType/ISQLTypeRemoteFileId.h>

//------------------------------------------------------------------------------
/**
  Реализация SQL-переменной, представляющий идентификатор удаленного файла на сервере
*/
//---
class PGSQLTypeRemoteFileId: public ISQLTypeRemoteFileId
{
	std::optional<std::string> m_id; ///< Идентификатор удаленного файла

public:
	/// Конструктор по умолчанию
	PGSQLTypeRemoteFileId() = default;
	/// Конструктор от аргумента
	PGSQLTypeRemoteFileId(const std::string & id);

public: // ISQLTypeRemoteFileId
	/// Получить идентификатор
	virtual const std::optional<std::string> & GetValue() const override;

public: // ISQLType
	/// Сконвертировать в строку
	virtual std::optional<std::string> ToSQLString() const override;
	/// Получить название SQL-типа
	virtual const std::string & GetTypeName() const override;

public:
	/// Прочитать значение из строки
	/// \param value Строка, из которой нужно читать значение.
	///              Если чтение прошло успешно, то строка будет очищена, иначе останется неизменной.
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFromSQL(std::string && value) override;

private:
	/// Строка валидна для установки в объект
	static bool isValid(const std::string & str);
};
