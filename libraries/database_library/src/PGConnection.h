#pragma once

#include <IConnection.h>

#include <libpq-fe.h>

#include <mutex>

class PGConnection;
/// Разделяемый владеющий указатель на PGConnection
using PGConnectionSPtr = std::shared_ptr<PGConnection>;
/// Разделяемый невладеющий указатель на PGConnection
using PGConnectionWPtr = std::weak_ptr<PGConnection>;

//------------------------------------------------------------------------------
/**
  Соединение с базой данных PostgreSQL.
*/
//---
class PGConnection : public std::enable_shared_from_this<PGConnection>, public IConnection
{
	PGconn * m_conn = nullptr; ///< Соединение
	std::mutex m_mutex; ///< Мьютекс для корректной работы в многопоточной среде
						///< (когда несколько потоков используют одно соединение)

	int m_transactionCount = 0; ///< Счетчик открытых транзакций

public:
	/// Деструктор
	virtual ~PGConnection() override;

	/// Создать экземпляр
	static PGConnectionSPtr Create(const std::string & connectionInfo);

private:
	/// Конструктор
	PGConnection(const std::string & connectionInfo);
	/// Конструктор копирования
	PGConnection(const PGConnection &) = delete;
	/// Конструктор перемещения
	PGConnection(PGConnection &&) = delete;
	/// Оператор присваивания копированием
	PGConnection& operator=(const PGConnection &) = delete;
	/// Оператор присваивания перемещением
	PGConnection& operator=(PGConnection &&) = delete;

public:
	/// Валидно ли соединение
	virtual bool IsValid() const override;
	/// Получить статус соединения
	virtual ConnectionStatus GetStatus() const override;

	/// Выполнить запрос. Запрос может содержать несколько SQL команд, тогда они будут выполнены в рамках одной транзакции
	/// (только если команды BEGIN/COMMIT не включены явно в запрос, чтобы разделить его на несколько транзакций)
	virtual IExecuteResultPtr Execute(const std::string & query) override;

	/// Открыть транзакцию.
	/// Метод увеличивает счетчик открытых транзакций.
	/// Транзакция создается только тогда, когда значение счетчика меняется с 0 на 1.
	virtual IExecuteResultStatusPtr BeginTransaction() override;

	/// Закрыть транзакцию.
	/// Метод уменьшает счетчик открытых транзакций.
	/// Транзакция закрывается только тогда, когда значение счетчика меняется с 1 на 0.
	virtual IExecuteResultStatusPtr EndTransaction() override;

private:
	/// Вспомогательный метод для реализации Execute. Не блокирует мьютекс.
	IExecuteResultPtr executeImpl(const std::string & query);

public:
	/// Создать удаленный файл
	virtual IFilePtr CreateRemoteFile() override;

protected:
	// todo: IConnection::Execute перегрузка с бинарными данными

	/// Выполнить запрос с аргументами. Запрос не может содержать более одной SQL-команды.
	/// \param query Строка в запросом. Аргументы в запросе обозначаются знаком доллара с последующим номером аргумента,
	///              начиная с единицы. Пример запроса: "INSERT INTO table VALUES($1, $2);".
	/// \param args Массив аргументов, каждый из которых может быть либо текстовой строкой, либо массивом байт.
	/// \param types Массив типов аргументов.
	///              Если передан пустой массив, то типы аргументов будут вычислены автоматически.
	///              Иначе будет предпринята попытка привести аргументы к требуемым типам.
	///              В качестве типа аргумента может быть указан неизвестный тип (SQLDataType::Unknown) - в этом
	///              случае тип этого аргумента будет вычислен автоматически.
	/// \param resultFormat Формат, в котором будет представлен результат (бинарный или текстовый)
	virtual IExecuteResultPtr Execute(const std::string & singleCommand,
		const std::vector<ExecuteArgType> & args,
		const ResultFormat resultFormat = ResultFormat::Text,
		const std::vector<SQLDataType> & types = {}) override;

public: // Методы для работы с большими бинарными объектами
		// todo: Добавить диагностику, возвращая дополнительное значение (генерируемое функцией PQerrorMessage)

	/// Создать большой бинарный объект
	Oid LoCreate();
	/// Открыть большой бинарный объект
	int LoOpen(Oid objId, int mode);
	/// Прочитать данные из большого бинарного объекта
	int LoRead(int fd, char * buffer, size_t len);
	/// Записать данные в большой бинарный объект
	int LoWrite(int fd, const char * data, size_t len);
	/// Закрыть большой бинарный объект
	int LoClose(int fd);
};
