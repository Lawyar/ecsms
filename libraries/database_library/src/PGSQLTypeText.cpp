#include "PGSQLTypeText.h"

#include <algorithm>

//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGSQLTypeText::PGSQLTypeText(std::string &&value) {
  SetValue(std::move(value));
}

//------------------------------------------------------------------------------
/**
  Получить значение
*/
//---
const std::optional<std::string> &PGSQLTypeText::GetValue() const {
  return m_value;
}

//------------------------------------------------------------------------------
/**
  Установить значение
  \param value Строка, из которой нужно установить значение.
               При успехе строка будет очищена, иначе останется неизменной.
*/
//---
void PGSQLTypeText::SetValue(std::string &&value) {
  if (std::count_if(value.begin(), value.end(), isInvalidChar) != 0) {
    // Если в строке есть недопустимые символы, то она невалидна.
    m_value = std::nullopt;
    return;
  }

  m_value = std::move(value);
}

//------------------------------------------------------------------------------
/**
  Сконвертировать в строку
*/
//---
std::optional<std::string> PGSQLTypeText::ToSQLString() const {
  if (!m_value)
    return std::nullopt;

  // Символ начала и конца SQL-строки. Внутри SQL строки его нужно экранировать
  // самим собой.
  static constexpr char specialChar = '\'';

  std::string result;

  result.reserve(m_value->size() +
                 static_cast<size_t>(2 + m_value->size() / 128));
  // + 2 байта на открывающий и закрывающий символы
  // + (size / 128) байт на апострофы
  // (исходя из расчета, что апострофы встречаются в среднем с частотой
  // 0.005...0.008)

  result.push_back(specialChar);
  for (auto ch : *m_value) {
    if (ch == specialChar) {
      // Экранируем спецсимвол
      result.push_back(specialChar);
      result.push_back(specialChar);
    } else {
      result.push_back(ch);
    }
  }
  result.push_back(specialChar);

  return result;
}

//------------------------------------------------------------------------------
/**
  Получить название SQL-типа
*/
//---
const std::string &PGSQLTypeText::GetTypeName() const {
  static const std::string name = "TEXT";
  return name;
}

//------------------------------------------------------------------------------
/**
  Прочитать значение из строки
  \param value Строка, из которой нужно читать значение.
               Если чтение прошло успешно, то строка будет очищена, иначе
               останется неизменной.
*/
//---
bool PGSQLTypeText::ReadFromSQL(std::string &&value) {
  SetValue(std::move(value));
  return m_value.has_value();
}

//------------------------------------------------------------------------------
/**
  Прочитать значение из массива байт
  \param value Массив, из которого нужно читать значение.
               Если чтение прошло успешно, то массив будет очищен, иначе
               останется неизменным.
*/
//---
bool PGSQLTypeText::ReadFromSQL(std::vector<char> &&value) {
  // Определяем валидность строки
  if (value.empty())
    // Пустой массив байт не конвертируется в пустую строку,
    // поскольку в строке обязательно должен быть один символ - нуль-терминатор
    // в конце строки. (то есть пустая строка представляется массивом байтов
    // так: {'\0'}). Если массив байт пустой, значит строка невалидна.
    return false;

  if (value.back() != '\0')
    // Если строка не оканчивается на нуль-терминатор, то она невалидна.
    return false;

  std::string str(value.data(), value.size() - 1);
  SetValue(std::move(str));

  bool result = m_value.has_value();
  if (result)
    value.clear();
  return result;
}

//------------------------------------------------------------------------------
/**
  Это недопустимый символ
*/
//---
bool PGSQLTypeText::isInvalidChar(char c) {
  // Возможно, надо также добавить проверки на наличие нечитаемых символов, но
  // пока и так сойдет
  return c == '\0';
}
