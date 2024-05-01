#pragma once

#include <string>
#include <string_view>
#include <memory>

#include <cstdio>
#include <cassert>

#include <type_traits>

namespace utils::string
{
	/// Перевести строку в нижний регистр
	std::string ToLower(const std::string & sourceStr);
	/// Перевести строку в верхний регистр
	std::string ToUpper(const std::string & sourceStr);
	///  Замена всех подстрок
	void ReplaceAll(std::string & str, const std::string & before, const std::string & after);

	/// Перевести в строку
	template <class T>
	std::string ToString(const T & value)
	{
		if constexpr (std::is_arithmetic_v<T>)
			return std::to_string(value);
		else
			return std::string(value);
	}


	//------------------------------------------------------------------------------
	/**
	  Форматированный вывод, похожий на std::format из C++20.
	  Последовательность "{}" заменяется на соответствующий аргумент. Например, вызов
	  Format("{}: x = {}, y = {}", "Values", 5, 6.0) возвращает строку
	  "Values: x = 5, y = 6.0".

	  Однако экранирование символов "{" и "}" последовательностями "{{" и "}}" (как в std::format)
	  не реализовано.
	  Если хочется написать в строке последовательность "{}", то можно сделать так:
	  Format("Some string with {} {}", "{}", "other arg") - функция вернет
	  "Some string with {} other arg".
	  Для написания одиночных символов "{" и "}" (если они не стоят рядом с другими символами "}" и "{",
	  образуя "{}") никаких дополнительных действий не требуется.
	  Format("{Some string with {}}", "arg") => "{Some string with arg}"

	  Для перевода аргумента в строку используется функция ToString.
	*/
	//---
	template<typename ... Args>
	std::string Format(const std::string & _format, Args ... args)
	{
		auto format = _format;
		ReplaceAll(format, "%", "%%"); // передаем в функцию snprintf, значит нужно экранировать проценты
		ReplaceAll(format, "{}", "%s");

		int sizeInt = std::snprintf(nullptr, 0, format.c_str(), ToString(args).c_str() ...);
		if (sizeInt <= 0)
		{
			assert(false);
			return {};
		}
		// Дополнительный символ для '\0'
		auto size = static_cast<size_t>(sizeInt + 1);
		std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, format.c_str(), ToString(args).c_str() ...);
		// Не берем '\0'
		return std::string(buf.get(), buf.get() + size - 1);
	}
}