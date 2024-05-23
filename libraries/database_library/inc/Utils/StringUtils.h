#pragma once

#include <memory>
#include <optional>
#include <string>

#include <cassert>
#include <cctype>
#include <cstdio>

#include <type_traits>

namespace utils::string {
/// Перевести строку в нижний регистр
std::string ToLower(const std::string &sourceStr);
/// Перевести строку в верхний регистр
std::string ToUpper(const std::string &sourceStr);
/// Строка содержит только цифры
bool HasOnlyDigits(const std::string &str);
///  Замена всех подстрок
void ReplaceAll(std::string &str, const std::string &before,
                const std::string &after);
/// Посчитать количество вхождений подстроки в строку
size_t GetSubstringsCount(const std::string &str, const std::string &substr);

/// Перевести в строку
template <class T> std::string ToString(const T &value) {
  if constexpr (std::is_arithmetic_v<T>)
    return std::to_string(value);
  else
    return std::string(value);
}

//------------------------------------------------------------------------------
/**
  Форматированный вывод, похожий на std::format из C++20.
  Последовательность "{}" заменяется на соответствующий аргумент. Например,
  вызов Format("{}: x = {}, y = {}", "Values", 5, 6.0) возвращает строку
  "Values: x = 5, y = 6.0".

  Однако экранирование символов "{" и "}" последовательностями "{{" и "}}" (как
  в std::format) не реализовано. Если хочется написать в строке
  последовательность "{}", то можно сделать так: Format("Some string with {}
  {}", "{}", "other arg") - функция вернет "Some string with {} other arg". Для
  написания одиночных символов "{" и "}" (если они не стоят рядом с другими
  символами "}" и "{", образуя "{}") никаких дополнительных действий не
  требуется. Format("{Some string with {}}", "arg") => "{Some string with arg}"

  Для перевода аргумента в строку используется функция ToString.
*/
//---
template <typename... Args>
std::string Format(const std::string &_format, Args... args) {
  if (sizeof...(args) != GetSubstringsCount(_format, "{}")) {
    assert(false);
    return "";
  }

  auto format = _format;
  ReplaceAll(
      format, "%",
      "%%"); // передаем в функцию snprintf, значит нужно экранировать проценты
  ReplaceAll(format, "{}", "%s");

  int sizeInt =
      std::snprintf(nullptr, 0, format.c_str(), ToString(args).c_str()...);
  if (sizeInt <= 0) {
    assert(false);
    return {};
  }
  // Дополнительный символ для '\0'
  auto size = static_cast<size_t>(sizeInt + 1);
  std::unique_ptr<char[]> buf(new char[size]);
  std::snprintf(buf.get(), size, format.c_str(), ToString(args).c_str()...);
  // Не берем '\0'
  return std::string(buf.get(), buf.get() + size - 1);
}

//------------------------------------------------------------------------------
/**
  Перевести в число (с использованием стандартной функции std::sto*).
  \param standardFunctor Стандартная функция для конвертации числа.
  \param str Строка
  \param base Основание системы счисления
  \return Число при успешной конвертации, иначе std::nullopt.
          Конвертация считается выполненной успешно, если строка содержала
          только символы, относящиеся к числу, и её удалось привести к числу.
*/
//---
template <class Functor>
auto StringToNumber(Functor &&standardFunctor, const std::string &str,
                    int base = 10)
    -> std::optional<
        decltype(standardFunctor(str, static_cast<size_t *>(nullptr), base))> {
  const bool startsWithDigit = std::isdigit(static_cast<unsigned char>(str[0]));
  const bool startsWithPlus = str[0] == '+';
  const bool startsWithMinus = str[0] == '-';
  if (!startsWithDigit && !startsWithPlus && !startsWithMinus)
    // Число должно начинаться с цифры, плюса или минуса
    return std::nullopt;

  try {
    std::size_t pos;
    auto result = standardFunctor(str, &pos, base);
    if (pos == str.size())
      return result;
  } catch (...) {
  }

  return std::nullopt;
}
} // namespace utils::string
