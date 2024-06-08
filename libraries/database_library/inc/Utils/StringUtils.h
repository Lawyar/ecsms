#pragma once

#include <string>
#include <memory>
#include <optional>

#include <cstdio>
#include <cassert>
#include <cctype>

#include <type_traits>

namespace utils::string
{
	/// ��������� ������ � ������ �������
	std::string ToLower(const std::string & sourceStr);
	/// ��������� ������ � ������� �������
	std::string ToUpper(const std::string & sourceStr);
	/// ������ �������� ������ �����
	bool HasOnlyDigits(const std::string & str);
	///  ������ ���� ��������
	void ReplaceAll(std::string & str, const std::string & before, const std::string & after);
	/// ��������� ���������� ��������� ��������� � ������
	size_t GetSubstringsCount(const std::string & str, const std::string & substr);

	/// ��������� � ������
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
	  ��������������� �����, ������� �� std::format �� C++20.
	  ������������������ "{}" ���������� �� ��������������� ��������. ��������, �����
	  Format("{}: x = {}, y = {}", "Values", 5, 6.0) ���������� ������
	  "Values: x = 5, y = 6.0".

	  ������ ������������� �������� "{" � "}" �������������������� "{{" � "}}" (��� � std::format)
	  �� �����������.
	  ���� ������� �������� � ������ ������������������ "{}", �� ����� ������� ���:
	  Format("Some string with {} {}", "{}", "other arg") - ������� ������
	  "Some string with {} other arg".
	  ��� ��������� ��������� �������� "{" � "}" (���� ��� �� ����� ����� � ������� ��������� "}" � "{",
	  ������� "{}") ������� �������������� �������� �� ���������.
	  Format("{Some string with {}}", "arg") => "{Some string with arg}"

	  ��� �������� ��������� � ������ ������������ ������� ToString.
	*/
	//---
	template<typename ... Args>
	std::string Format(const std::string & _format, Args ... args)
	{
		if (sizeof...(args) != GetSubstringsCount(_format, "{}"))
		{
			assert(false);
			return "";
		}

		auto format = _format;
		ReplaceAll(format, "%", "%%"); // �������� � ������� snprintf, ������ ����� ������������ ��������
		ReplaceAll(format, "{}", "%s");

		int sizeInt = std::snprintf(nullptr, 0, format.c_str(), ToString(args).c_str() ...);
		if (sizeInt <= 0)
		{
			assert(false);
			return {};
		}
		// �������������� ������ ��� '\0'
		auto size = static_cast<size_t>(sizeInt + 1);
		std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, format.c_str(), ToString(args).c_str() ...);
		// �� ����� '\0'
		return std::string(buf.get(), buf.get() + size - 1);
	}


	//------------------------------------------------------------------------------
	/**
	  ��������� � ����� (� �������������� ����������� ������� std::sto*).
	  \param standardFunctor ����������� ������� ��� ����������� �����.
	  \param str ������
	  \param base ��������� ������� ���������
	  \return ����� ��� �������� �����������, ����� std::nullopt.
	          ����������� ��������� ����������� �������, ���� ������ ��������� ������
			  �������, ����������� � �����, � � ������� �������� � �����.
	*/
	//---
	template <class Functor>
	auto StringToNumber(Functor && standardFunctor, const std::string & str, int base = 10)
		-> std::optional<decltype(standardFunctor(str, static_cast<size_t *>(nullptr), base))>
	{
		const bool startsWithDigit = std::isdigit(static_cast<unsigned char>(str[0]));
		const bool startsWithPlus = str[0] == '+';
		const bool startsWithMinus = str[0] == '-';
		if (!startsWithDigit && !startsWithPlus && !startsWithMinus)
			// ����� ������ ���������� � �����, ����� ��� ������
			return std::nullopt;

		try
		{
			std::size_t pos;
			auto result = standardFunctor(str, &pos, base);
			if (pos == str.size())
				return result;
		}
		catch (...)
		{
		}

		return std::nullopt;
	}
}
