#pragma once

#include <IConnection.h>

#include <libpq-fe.h>

#include <mutex>

//------------------------------------------------------------------------------
/**
  —оединение с базой данных PostgreSQL.
*/
//---
class PGConnection : public IConnection
{
	PGconn * m_conn = nullptr; ///< —оединение
	std::mutex m_mutex; ///< ћьютекс дл€ корректной работы в многопоточной среде
						///< (когда несколько потоков используют одно соединение)
public:
	///  онструктор
	PGConnection(const std::string & connectionInfo);
	/// ƒеструктор
	~PGConnection();
	///  онструктор копировани€
	PGConnection(const PGConnection &) = delete;
	///  онструктор перемещени€
	PGConnection(PGConnection &&) = delete;
	/// ќператор присваивани€ копированием
	PGConnection& operator=(const PGConnection &) = delete;
	/// ќператор присваивани€ перемещением
	PGConnection& operator=(PGConnection &&) = delete;

public:
	/// ¬алидно ли соединение
	virtual bool IsValid() const override;
	/// ѕолучить статус соединени€
	virtual ConnectionStatus GetStatus() const override;

	/// ¬ыполнить запрос. «апрос может содержать несколько SQL команд, тогда они будут выполнены в рамках одной транзакции
	/// (только если команды BEGIN/COMMIT не включены €вно в запрос, чтобы разделить его на несколько транзакций)
	virtual IExecuteResultPtr Execute(const std::string & query) override;

protected:
	// todo: IConnection::Execute перегрузка с бинарными данными

	/// ¬ыполнить запрос с аргументами. «апрос не может содержать более одной SQL-команды.
	/// \param query —трока в запросом. јргументы в запросе обозначаютс€ знаком доллара с последующим номером аргумента,
	///              начина€ с единицы. ѕример запроса: "INSERT INTO table VALUES($1, $2);".
	/// \param args ћассив аргументов, каждый из которых может быть либо текстовой строкой, либо массивом байт.
	/// \param types ћассив типов аргументов.
	///              ≈сли передан пустой массив, то типы аргументов будут вычислены автоматически.
	///              »наче будет предприн€та попытка привести аргументы к требуемым типам.
	///              ¬ качестве типа аргумента может быть указан неизвестный тип (SQLDataType::Unknown) - в этом
	///              случае тип этого аргумента будет вычислен автоматически.
	/// \param resultFormat ‘ормат, в котором будет представлен результат (бинарный или текстовый)
	virtual IExecuteResultPtr Execute(const std::string & singleCommand,
		const std::vector<ExecuteArgType> & args,
		const ResultFormat resultFormat = ResultFormat::Text,
		const std::vector<SQLDataType> & types = {}) override;
};
