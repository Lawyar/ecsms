#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <DataType/SQLDataType.h>

//------------------------------------------------------------------------------
/**
  \brief Интерфейс SQL-типа данных
*/
//---
class ISQLType {
public:
  /// Деструктор
  virtual ~ISQLType() = default;

public:
  /// Сконвертировать переменную в представление, в котором она может быть
  /// использована в качестве аргумента в SQL-запросе.
  /// \return SQL-строка, представляющая данную переменную, если переменная
  ///         непустая, иначе \c std::nullopt.
  virtual std::optional<std::string> ToSQLString() const = 0;
  /// Получить название SQL-типа
  virtual const std::string &GetTypeName() const = 0;

  /// Проверить, является ли переменная пустой.
  /// Переменная является пустой сразу после её создания.
  /// Также переменная может стать пустой после неудачной операции
  /// чтения (ReadFromSQL).
  /// \return \c true, если переменная пустая, иначе \c false.
  bool IsEmpty() const { return !ToSQLString().has_value(); }

public:
  /// Получить тип данных
  virtual SQLDataType GetType() const = 0;

public:
  /// Прочитать значение из строки, полученной из результата SQL-запроса
  /// \param value Строка, из которой нужно читать значение.
  ///              Если чтение прошло успешно, то строка будет очищена, иначе
  ///              останется неизменной.
  /// \return Валидно ли прочитанное значение
  virtual bool ReadFromSQL(std::string &&value) = 0;

protected:
  /// Прочитать значение из массива байт, полученного из результата SQL-запроса
  /// \todo В данный момент эта перегрузка недоступна, потому что закрыта
  /// возможность отправлять запрос,
  /// \ref IConnection_Execute_Binary "запрашивая результат в бинарном виде"
  /// \param value Массив, из которого нужно читать значение.
  ///              Если чтение прошло успешно, то массив будет очищен, иначе
  ///              останется неизменным.
  /// \return Валидно ли прочитанное значение
  virtual bool ReadFromSQL(std::vector<char> &&value) { return false; }
};

/// Тип указателя на ISQLType
using ISQLTypePtr = std::shared_ptr<ISQLType>;
