#pragma once

#include <DataType/SQLDataType.h>
#include <IExecuteResultStatus.h>

#include <optional>
#include <string>
#include <variant>
#include <vector>

//------------------------------------------------------------------------------
/**
  Формат данных в результате
*/
//---
enum class ResultFormat
{
	Text,   ///< Формат представлен в виде текстовых данных
	Binary, ///< Формат представлен в виде бинарных данных
	Invalid ///< Невалидный формат данных
};

//------------------------------------------------------------------------------
/**
  Интерфейс результата запроса к базе данных.
  Во всех методах индексация начинается с нуля.
*/
//---
class IExecuteResult
{
public:
	/// Тип данных ячейки таблицы. Если ячейка содержит null или отсутствует, то значение ячейки - std::nullopt,
	/// в противном случае ячейка содержит текстовые или бинарные данные
	/// (в зависимости от того, какой тип данных запрашивался в команде).
	// todo : Сделать optional<...> полем класса
	struct CellType : public std::optional<std::variant<std::string, std::vector<char>>>
	{
		using std::optional<std::variant<std::string, std::vector<char>>>::operator=;
		/// Ячейка содержит строку
		bool HasString() const { return has_value() && std::holds_alternative<std::string>(value()); }
		/// Ячейка содержит бинарные данные
		bool HasByteArray() const { return has_value() && std::holds_alternative<std::vector<char>>(value()); }
		/// Ячейка содержит пустое значение или ячейка отсутствует
		bool HasNull() const { return !has_value(); }

		/// Получить строку, если данные представлены строкой
		std::string GetString() const { return HasString() ? std::get<std::string>(value()) : std::string(); }
		/// Получить массив байт, если данные представлены массивом байт
		std::vector<char> GetByteArray() const { return HasByteArray() ? std::get<std::vector<char>>(value()) : std::vector<char>(); }
	};

public:
	static constexpr size_t InvalidIndex = (size_t)(-1); ///< Невалидный индекс
public:
	/// Деструктор
	virtual ~IExecuteResult() = default;

public:
	/// Получить текущий статус запроса
	virtual IExecuteResultStatusPtr GetCurrentExecuteStatus() const = 0;

public:
	/// Получить количество строк в ответе
	virtual size_t GetRowCount() const = 0;
	/// Получить количество столбцов в ответе
	virtual size_t GetColCount() const = 0;
	
	/// Получить название столбца (возвращает пустую строку, если передан невалидный индекс)
	virtual std::string GetColName(size_t columnIndex) const = 0;
	/// Получить индекс столбца по имени (возвращает невалидный индекс, если передано несуществующее имя)
	virtual size_t GetColIndex(const std::string & columnName) = 0;
	
	/// Получить тип данных столбца (возвращает невалидный тип данных, если передан невалидный индекс)
	virtual SQLDataType GetColType(size_t columnIndex) const = 0;

	/// Получить значение.
	/// Возвращает std::nullopt, если ячейка содержит null или если переданы невалидные индексы.
	/// В противном случае возвращает текст или бинарные данные
	/// (в зависимости от того, в каком виде запрашивались данные в команде)
	virtual CellType GetValue(size_t rowIndex, size_t columnIndex) = 0;
};

/// Указатель на IExecuteResult
using IExecuteResultPtr = std::shared_ptr<IExecuteResult>;
