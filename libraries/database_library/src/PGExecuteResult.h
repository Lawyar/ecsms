#pragma once

#include <IExecuteResult.h>
#include <libpq-fe.h>

//------------------------------------------------------------------------------
/**
  Класс результата запроса к базе данных PostgreSQL.
*/
//---
class PGExecuteResult : public IExecuteResult
{
	PGresult * m_result = nullptr; ///< Результат запроса
public:
	/// Конструктор
	PGExecuteResult(PGresult * result);
	/// Деструктор
	virtual ~PGExecuteResult() override;
	/// Конструктор копирования
	PGExecuteResult(const PGExecuteResult &) = delete;
	/// Конструктор перемещения
	PGExecuteResult(PGExecuteResult &&) = delete;
	/// Оператор присваивания копированием
	PGExecuteResult& operator=(const PGExecuteResult &) = delete;
	/// Оператор присваивания перемещением
	PGExecuteResult& operator=(PGExecuteResult &&) = delete;

public:
	/// Получить текущий статус запроса
	virtual IExecuteResultStatusPtr GetCurrentExecuteStatus() const override;

public:
	/// Получить количество строк в ответе
	virtual size_t GetRowCount() const override;
	/// Получить количество столбцов в ответе
	virtual size_t GetColCount() const override;
	
	/// Получить название столбца (возвращает пустую строку, если передан невалидный индекс)
	virtual std::string GetColName(size_t columnIndex) const override;
	/// Получить индекс столбца по имени (возвращает невалидный индекс, если передано несуществующее имя)
	virtual size_t GetColIndex(const std::string & columnName) override;
	
	/// Получить тип данных столбца (возвращает невалидный тип данных, если передан невалидный индекс)
	virtual SQLDataType GetColType(size_t columnIndex) const override;

	/// Получить значение.
	virtual CellType GetValue(size_t rowIndex, size_t columnIndex) override;
};
