#pragma once

#include <DataType/ISQLTypeConverter.h>
#include <IConnection.h>
#include <IExecutorEAV.h>

#include <map>
#include <memory>
#include <string>

//------------------------------------------------------------------------------
/**
  \brief Интерфейс менеджера базы данных.

  Позволяет получить доступ к основным интерфейсам для работы с системой
  хранения данных: IConnection, ISQLTypeConverter, IExecutorEAV
*/
//---
class IDatabaseManager {
public:
  /// Деструктор
  virtual ~IDatabaseManager() = default;

public:
  /// Получить соединение
  /// \param connectionInfo Строка специального вида, содержащая информацию для
  ///        установления соединения.
  ///        Пример: "postgresql://user:password@127.0.0.1:5432/database_name".
  /// \return Указатель на интерфейс соединения.
  virtual IConnectionPtr GetConnection(const std::string &connectionInfo) = 0;

  /// Получить исполнитель EAV-запросов
  /// \param connection Соединение, по которому должен работать исполнитель
  ///                   EAV-запросов.
  /// \return Указатель на интерфейс исполнителя EAV-запросов.
  virtual IExecutorEAVPtr GetExecutorEAV(const IConnectionPtr &connection) = 0;

  /// Получить конвертер SQL-типов
  /// \return Указатель на интерфейс конвертера SQL-типов.
  virtual ISQLTypeConverterPtr GetSQLTypeConverter() const = 0;
};

/// Тип базы данных
enum class DatabaseType {
  PostgreSQL ///< База данных PostgreSQL
};

/// Получить менеджер базы данных
/// \tparam Тип базы данных, менеджер которой нужно получить.
/// \return Ссылка на интерфейс базы данных.
template <DatabaseType type = DatabaseType::PostgreSQL>
extern IDatabaseManager &GetDatabaseManager();
