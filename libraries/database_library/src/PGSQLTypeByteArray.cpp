#include "PGSQLTypeByteArray.h"

#include <algorithm>

//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGSQLTypeByteArray::PGSQLTypeByteArray(std::vector<char> &&value)
    : m_value(std::move(value)) {}

//------------------------------------------------------------------------------
/**
  Получить значение
*/
//---
const std::optional<std::vector<char>> &PGSQLTypeByteArray::GetValue() const {
  return m_value;
}

//------------------------------------------------------------------------------
/**
  Установить значение
  \param value Массив, из которого нужно установить значение.
               При успехе массив будет очищен, иначе останется неизменным.
*/
//---
void PGSQLTypeByteArray::SetValue(std::vector<char> &&value) {
  m_value = std::move(value);
}

//------------------------------------------------------------------------------
/**
  Установить значение
*/
//---
std::optional<std::string> PGSQLTypeByteArray::ToSQLString() const {
  if (!m_value)
    return std::nullopt;

  static const char codes[17] = "0123456789abcdef";
  const std::string postfix =
      "::" + GetTypeName(); // Явное указание типа SQL-литерала

  std::string result;
  // Каждый байт займет по два символа + 4 байта на символы '\x' + байты на
  // постфикс
  result.reserve(m_value->size() * 2 + 4UL + postfix.size());

  result.push_back('\'');
  result.push_back('\\');
  result.push_back('x');
  for (char ch : *m_value) {
    char high =
        codes[static_cast<size_t>((ch & 0b11110000) >> 4)]; // старший разряд
    char low = codes[static_cast<size_t>(ch & 0b1111)]; // младший разряд
    result.push_back(high);
    result.push_back(low);
  }
  result.push_back('\'');
  result += postfix;

  return result;
}

//------------------------------------------------------------------------------
/**
   Получить название SQL-типа
*/
//---
const std::string &PGSQLTypeByteArray::GetTypeName() const {
  static const std::string name = "BYTEA";
  return name;
}

//------------------------------------------------------------------------------
/**
  Получить массив для перевода шестнадцатиричных кодов в числа
*/
//---
static std::vector<int> GetCodeToValueArr() {
  std::vector<int> arr(256, 0);
  for (int i = 0; i <= 9; ++i) {
    char c = '0' + i;
    arr[static_cast<unsigned char>(c)] = i;
  }

  for (int i = 0; i <= 5; ++i) {
    char c1 = 'a' + i;
    char c2 = 'A' + i;
    arr[static_cast<unsigned char>(c1)] = 10 + i;
    arr[static_cast<unsigned char>(c2)] = 10 + i;
  }

  return arr;
}

//------------------------------------------------------------------------------
/**
   Прочитать значение из строки
   \param value Строка, из которой нужно читать значение.
                Если чтение прошло успешно, то строка будет очищена, иначе
                останется неизменной.
*/
//---
bool PGSQLTypeByteArray::ReadFromSQL(std::string &&value) {
  m_value = std::nullopt;

  if (value.size() < 2)
    // Строка, возвращенная из SQL-результата должна начинаться с \x
    return false;

  if (value.substr(0, 2) != "\\x")
    // Проверяем, что в начале действительно идет \x
    return false;

  if (value.size() % 2 != 0)
    // В строке должно быть четное количество символов.
    // (в строке байты представляются двумя шестнадцатиричными числами)
    return false;

  if (!std::all_of(std::next(value.begin(), 2), value.end(), [](char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
               (c >= 'A' && c <= 'F');
      })) {
    // В оставшейся строке (после \x) должны идти только шестнадцатиричные цифры
    // в виде символов
    return false;
  }

  const std::vector<int> values = GetCodeToValueArr();

  std::vector<char> arr;
  arr.reserve((value.size() - 2) / 2);

  for (size_t i = 2; i < value.size(); i += 2) {
    char highChar = value[i];
    char lowChar = value[i + 1];
    int high = values[static_cast<unsigned char>(highChar)];
    int low = values[static_cast<unsigned char>(lowChar)];

    char byte = ((high << 4) | low);
    arr.push_back(byte);
  }

  m_value = std::move(arr);
  value.clear();
  return true;
}

//------------------------------------------------------------------------------
/**
   Прочитать значение из массива байт
   \param value Массив, из которого нужно читать значение.
                Если чтение прошло успешно, то массив будет очищен, иначе
                останется неизменным.
*/
//---
bool PGSQLTypeByteArray::ReadFromSQL(std::vector<char> &&value) {
  SetValue(std::move(value));
  return m_value.has_value();
}
