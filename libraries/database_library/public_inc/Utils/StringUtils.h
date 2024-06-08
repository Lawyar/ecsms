#pragma once

#include <string>
#include <string_view>
#include <memory>

#include <cstdio>
#include <cassert>

#include <type_traits>

namespace utils::string
{
	/// ��������� ������ � ������ �������
	std::string ToLower(const std::string & sourceStr);
	/// ��������� ������ � ������� �������
	std::string ToUpper(const std::string & sourceStr);
	///  ������ ���� ��������
	void ReplaceAll(std::string & str, const std::string & before, const std::string & after);

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
}