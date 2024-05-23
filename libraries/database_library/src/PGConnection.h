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
class PGConnection : public std::enable_shared_from_this<PGConnection>,
                     public IConnection {
  PGconn *m_conn = nullptr; ///< Соединение
  std::mutex m_mutex; ///< Мьютекс для корректной работы в многопоточной среде
                      ///< (когда несколько потоков используют одно соединение)

public:
  /// Деструктор
  virtual ~PGConnection() override;

  /// Создать экземпляр
  static PGConnectionSPtr Create(const std::string &connectionInfo);

private:
  /// Конструктор
  PGConnection(const std::string &connectionInfo);
  /// Конструктор копирования
  PGConnection(const PGConnection &) = delete;
  /// Конструктор перемещения
  PGConnection(PGConnection &&) = delete;
  /// Оператор присваивания копированием
  PGConnection &operator=(const PGConnection &) = delete;
  /// Оператор присваивания перемещением
  PGConnection &operator=(PGConnection &&) = delete;

public:
  /// Валидно ли соединение
  virtual bool IsValid() const override;
  /// Получить статус соединения
  virtual ConnectionStatus GetStatus() const override;

  /// Выполнить запрос.
  /// Запрос может содержать несколько SQL команд, тогда они автоматически будут
  /// выполнены в рамках одной транзакции, кроме случаев, когда команды для
  /// разбиения на транзакции не присутствуют явно (посредством добавления
  /// команд BEGIN/COMMIT в текст запроса query или посредством вызова методом
  /// BeginTransaction/CommitTransaction).
  virtual IExecuteResultPtr Execute(const std::string &query) override;

  /// Открыть транзакцию
  virtual IExecuteResultStatusPtr BeginTransaction() override;
  /// Закрыть транзакцию с применением изменений
  virtual IExecuteResultStatusPtr CommitTransaction() override;
  /// Отменить транзакцию (без применения изменений)
  virtual IExecuteResultStatusPtr RollbackTransaction() override;

private:
  /// Вспомогательный метод для реализации Execute. Не блокирует мьютекс.
  IExecuteResultPtr executeImpl(const std::string &query);

public:
  /// Создать удаленный файл.
  /// Метод может вызываться как внутри транзакции, так и не в ней.
  /// При вызове внутри транзакции действуют обычные правила транзакции:
  /// если транзакция не была завершена фиксацией или была завершена отменой, то
  /// файл не создатся. ПРЕДУПРЕЖЕДЕНИЕ: Последующая работа с удаленным файлом
  /// возможна только в рамках транзакции.
  virtual IFilePtr CreateRemoteFile() override;
  /// Удалить файл.
  /// Метод может вызываться как в рамках транзакции, так и не в них.
  /// При вызове внутри транзакции действуют обычные правила транзакции:
  /// если транзакция не была завершена фиксацией или была завершена отменой, то
  /// файл не создатся.
  virtual bool DeleteRemoteFile(const std::string &filename) override;
  /// Получить файл.
  /// ПРЕДУПРЕЖЕДЕНИЕ: Последующая работа с удаленным файлом возможна только в
  /// рамках транзакции.
  virtual IFilePtr GetRemoteFile(const std::string &filename) override;

protected:
  // todo: IConnection::Execute перегрузка с бинарными данными

  /// Выполнить запрос с аргументами. Запрос не может содержать более одной
  /// SQL-команды.
  virtual IExecuteResultPtr
  Execute(const std::string &singleCommand,
          const std::vector<ExecuteArgType> &args,
          const ResultFormat resultFormat = ResultFormat::Text,
          const std::vector<SQLDataType> &types = {}) override;

public: // Методы для работы с большими бинарными объектами
        // todo: Добавить диагностику, возвращая дополнительное значение
        // (генерируемое функцией PQerrorMessage)
  /// Создать большой бинарный объект
  Oid LoCreate();
  /// Удалить большой бинарный объект
  int LoUnlink(Oid objId);
  /// Открыть большой бинарный объект
  int LoOpen(Oid objId, int mode);
  /// Прочитать данные из большого бинарного объекта
  int LoRead(int fd, char *buffer, size_t len);
  /// Записать данные в большой бинарный объект
  int LoWrite(int fd, const char *data, size_t len);
  /// Переместиться в большом бинарном объекте
  pg_int64 LoLseek64(int fd, pg_int64 offset, int whence);
  /// Получить текущее положение в большом бинарном объекте
  pg_int64 LoTell64(int fd);
  /// Усечь (или расширить) большой бинарный объект
  pg_int64 LoTruncate64(int fd, pg_int64 len);
  /// Закрыть большой бинарный объект
  int LoClose(int fd);

private:
  /// Перевод названия файла в числовой идентификатор
  std::optional<Oid> fileNameToOid(const std::string &filename);
};
