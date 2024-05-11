#pragma once

#include <IExecuteResult.h>
#include <IFile.h>
#include <DataType/SQLDataType.h>

#include <memory>
#include <variant>

//------------------------------------------------------------------------------
/**
  Статус соединения
*/
//---
enum class ConnectionStatus
{
	Ok,     ///< Соединение успешно установлено
	Bad,    ///< Ошибка установки соединения
	Unknown ///< Неизвестный статус
};

/// Тип аргумента для выполнения метода Execute
using ExecuteArgType = std::variant<std::string, std::vector<char>>;

//------------------------------------------------------------------------------
/**
  Интерфейс соединения.
  При многопоточной работе с БД рекомендуется для каждого потока создавать
  свой объект соединения (в противном случае потоки будут выстраиваться в очередь).
*/
//---
class IConnection
{
public:
	/// Деструктор
	virtual ~IConnection() = default;

public:
	/// Валидно ли соединение
	virtual bool IsValid() const = 0;
	/// Получить статус соединения
	virtual ConnectionStatus GetStatus() const = 0;

	/// Выполнить запрос. Запрос может содержать несколько SQL команд, тогда они будут выполнены в рамках одной транзакции
	/// (только если команды BEGIN/COMMIT не включены явно в запрос, чтобы разделить его на несколько транзакций)
	virtual IExecuteResultPtr Execute(const std::string & query) = 0;

	/// Открыть транзакцию.
	/// Метод увеличивает счетчик открытых транзакций.
	/// Транзакция создается только тогда, когда значение счетчика меняется с 0 на 1.
	virtual IExecuteResultStatusPtr BeginTransaction() = 0;

	/// Закрыть транзакцию.
	/// Метод уменьшает счетчик открытых транзакций.
	/// Транзакция закрывается только тогда, когда значение счетчика меняется с 1 на 0.
	virtual IExecuteResultStatusPtr EndTransaction() = 0;

public:
	/// Создать удаленный файл
	virtual IFilePtr CreateRemoteFile() = 0;

protected:
	// todo: IConnection::Execute перегрузка с бинарными данными
	// Данный код на данный момент не используется и поэтому не тестировался,
	// так что перемещу его пока в protected-секцию.

	/// Выполнить запрос с аргументами. Запрос не может содержать более одной SQL-команды.
	/// \param query Строка в запросом. Аргументы в запросе обозначаются знаком доллара с последующим номером аргумента,
	///              начиная с единицы. Пример запроса: "INSERT INTO table VALUES($1, $2);".
	/// \param args Массив аргументов, каждый из которых может быть либо текстовой строкой, либо массивом байт.
	/// \param resultFormat Формат, в котором будет представлен результат (бинарный или текстовый)
	/// \param types Массив типов аргументов.
	///              Если передан пустой массив, то типы аргументов будут вычислены автоматически.
	///              Иначе будет предпринята попытка привести аргументы к требуемым типам.
	///              В качестве типа аргумента может быть указан неизвестный тип (SQLDataType::Unknown) - в этом
	///              случае тип этого аргумента будет вычислен автоматически.
	virtual IExecuteResultPtr Execute(const std::string & singleCommand,
		const std::vector<ExecuteArgType> & args,
		const ResultFormat resultFormat = ResultFormat::Text,
		const std::vector<SQLDataType> & types = {}) = 0;
};

/// Указатель на IConnection
using IConnectionPtr = std::shared_ptr<IConnection>;
