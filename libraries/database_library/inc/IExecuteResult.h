#pragma once

#include <DataType/SQLDataType.h>
#include <IExecuteResultStatus.h>

#include <optional>
#include <string>
#include <variant>
#include <vector>


//------------------------------------------------------------------------------
/**
  \brief Формат данных в результате.
*/
//---
enum class ResultFormat {
  Text, ///< Формат представлен в виде текстовых данных
  Binary, ///< Формат представлен в виде бинарных данных
  Invalid ///< Невалидный формат данных
};

//------------------------------------------------------------------------------
/**
  \brief Интерфейс для взаимодействия с результатом запроса к базе данных.
*/
//---
class IExecuteResult {
public:
  /// \brief Ячейка таблицы.
  ///
  /// Ячейка содержит тот тип данных (бинарный или текстовый), который
  /// запрашивался.
  class CellType {
    std::optional<std::variant<std::string, std::vector<char>, nullptr_t>>
        m_value; ///< Значение

  public:
    /// Конструктор
    CellType() : m_value() {}
    /// Конструктор от nullptr
    CellType(nullptr_t) : m_value(nullptr) {}
    /// Конструктор от строки
    CellType(const std::string &str) : m_value(str) {}

    /// Конструктор копирования
    CellType(const CellType &) = delete;
    /// Конструктор перемещения
    CellType(CellType &&) = default;
    /// Оператор присваивания копированием
    CellType &operator=(const CellType &) = delete;
    /// Оператор присваивания перемещением
    CellType &operator=(CellType &&) = default;

    /// Проверить содержит ли ячейка значение
    /// \return \c true, если в ячейке есть значение (строковое или \c null),
    ///         иначе \c false.
    bool HasValue() const { return !!m_value; }
    /// Проверить, содержит ли ячейка строку
    /// \return \c true, если ячейка содержит строку, иначе \c false.
    ///         Строка может быть при этом пустой.
    bool HasString() const {
      return m_value && std::holds_alternative<std::string>(*m_value);
    }
    /// Проверить, содержит ли ячейка null
    /// \return \c true, если ячейка содержит null, иначе \c false.
    bool HasNull() const {
      return m_value && std::holds_alternative<nullptr_t>(*m_value);
    }

    /// Извлечь строку, если данные представлены строкой
    /// \return Извлеченная из ячейки строка, если данные были представлены
    ///         строкой, в противном случае пустая строка.
    /// \warning Если данные были представлены строкой, то после её извлечения
    ///          ячейка станет пустой.
    std::string ExtractString() {
      std::string result;
      if (HasString()) {
        result = std::move(std::get<std::string>(*m_value));
        m_value = std::nullopt;
      }
      return std::move(result);
    }

  protected:
    /// Конструктор от массива байтов
    /// \todo Сейчас нет возможности получить ячейку, содержащую массив байтов,
    /// поскольку закрыта возможность
    /// \ref IConnection_Execute_Binary "запросить результат в бинарном виде"
    CellType(const std::vector<char> &arr) : m_value(arr) {}

    /// Проверить, содержит ли ячейка бинарные данные
    /// \return \c true, если ячейка содержит массив бинарных данных, иначе \c false.
    ///         Массив с бинарными данными может быть пустым.
    bool HasByteArray() const {
      return m_value && std::holds_alternative<std::vector<char>>(*m_value);
    }

    /// Извлечь массив байт, если данные представлены массивом байтов
    /// \return Извлеченный из ячейки массив байтов, если данные были
    ///         представлены массивом байтлв, в противном случае пустой массив
    ///         байтов.
    /// \warning Если данные были представлены массивом байтов, то после его
    ///          извлечения ячейка станет пустой.
    std::vector<char> ExtractByteArray() {
      std::vector<char> result;
      if (HasByteArray()) {
        result = std::move(std::get<std::vector<char>>(*m_value));
        m_value = std::nullopt;
      }
      return std::move(result);
    }
  };

public:
  static constexpr size_t InvalidIndex = (size_t)(-1); ///< Невалидный индекс

public:
  /// Деструктор
  virtual ~IExecuteResult() = default;

public:
  /// Получить текущий статус запроса
  /// \todo На данный момент все запросы выполняются синхронно, так что этот
  ///       метод всегда возвращает конечный статус выполнения запроса.
  ///       Однако в будущем возможно появление запросов, которые будут
  ///       выполняться асинхронно. Тогда этот метод будет возвращать текущий
  ///       статус выполнения асинхронного запроса: если запрос ещё не был
  ///       выполнен - то это статус ResultStatus::InProgress.
  /// \return Текущий статус запроса.
  virtual IExecuteResultStatusPtr GetCurrentExecuteStatus() const = 0;

public:
  /// Получить количество строк в ответе
  /// \return Количество строк в ответе.
  virtual size_t GetRowCount() const = 0;
  /// Получить количество столбцов в ответе
  /// \return Количество столбцов в ответе
  virtual size_t GetColCount() const = 0;

  /// Получить название столбца
  /// \param columnIndex Индекс столбца, название которого требуется получить.
  ///                    Индексация начинается с нуля.
  /// \return Название столбца, если передан валидный индекс, иначе пустую
  ///         строку.
  virtual std::string GetColName(size_t columnIndex) const = 0;
  /// Получить индекс столбца по имени
  /// \param columnName Имя столбца. Имя (в соотвествии со стандартом SQL) по
  ///                   умолчанию приводится к нижнему регистру, так что вызовы
  ///                   GetColIndex("columnname") и GetColIndex("CoLuMnNaMe")
  ///                   равнозначны. Исключением является имя, обрамленное
  ///                   кавычками. Так, если запрос содержал явное именование
  ///                   столбцов с использованием кавычек (например, SELECT 1 AS
  ///                   "Number"), то в эту функцию надо передавать
  ///                   GetColIndex("\"Number\"") для определения индекса
  ///                   столбца, а вызовы GetColIndex("Number"), или
  ///                   GetColIndex("\"number\"") вернут \ref InvalidIndex.
  /// \return Валидный индекс столбца с таким названием, если передано название
  ///         столбца, которое содержится в таблице-результате; иначе \ref InvalidIndex.
  virtual size_t GetColIndex(const std::string &columnName) = 0;

  /// Получить тип данных столбца
  /// \param columnIndex Индекс столбца, название которого требуется получить.
  ///                    Индексация начинается с нуля.
  /// \return Тип данных столбца, если передан валидный индекс, иначе
  ///         невалидный тип данных.
  virtual SQLDataType GetColType(size_t columnIndex) const = 0;

  /// Получить значение.
  /// \param rowIndex Индекс строка, значение которой требуется получить.
  ///                 Индексация начинается с нуля.
  /// \param columnIndex Индекс столбца, значение которого требуется получить.
  ///                    Индексация начинается с нуля.
  /// \return Ячейка со значением, если переданы валидные индексы, иначе
  ///         пустая ячейка.
  virtual CellType GetValue(size_t rowIndex, size_t columnIndex) = 0;
};

/// Указатель на IExecuteResult
using IExecuteResultPtr = std::shared_ptr<IExecuteResult>;
