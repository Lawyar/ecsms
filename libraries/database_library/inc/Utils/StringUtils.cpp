#include "StringUtils.h"

#include <algorithm>

//------------------------------------------------------------------------------
/**
  Перевести строку в нижний регистр
*/
//---
std::string utils::string::ToLower(const std::string & sourceStr)
{
	std::string str;
	std::transform(sourceStr.begin(), sourceStr.end(), std::back_inserter(str),
		// Это безопасный способ использования tolower
		// (https://en.cppreference.com/w/cpp/string/byte/tolower)
		[](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
	return str;
}


//------------------------------------------------------------------------------
/**
  Перевести строку в верхний регистр
*/
//---
std::string utils::string::ToUpper(const std::string & sourceStr)
{
	std::string str;
	std::transform(sourceStr.begin(), sourceStr.end(), std::back_inserter(str),
		// Это безопасный способ использования toupper
		// (https://en.cppreference.com/w/cpp/string/byte/toupper)
		[](char c) { return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); });
	return str;
}


//------------------------------------------------------------------------------
/**
  Строка содержит только цифры
*/
//---
bool utils::string::HasOnlyDigits(const std::string & str)
{
	return std::all_of(str.begin(), str.end(),
		// Это безопасный способ использования isdigit
		// (https://en.cppreference.com/w/cpp/string/byte/isdigit)
		[](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
}


//------------------------------------------------------------------------------
/**
	Замена всех подстрок
	\param str Строка, в которой производится замена
	\param before Подстрока, которую заменяем
	\param after Подстрока, на которую заменяем
*/
//---
void utils::string::ReplaceAll(std::string & str, const std::string & before, const std::string & after)
{
	if (before.size() > str.size())
		return;

	std::string::size_type currentPos = 0;
	for (auto findPos = str.find(before, currentPos); findPos != std::string::npos;
		currentPos = findPos + after.size(),
		findPos = str.find(before, currentPos))
	{
		str.replace(findPos, before.size(), after);
	}
}


//------------------------------------------------------------------------------
/**
	Посчитать количество вхождений подстроки в строку
	\param str Строка
	\param substr Подстрока
*/
//---
size_t utils::string::GetSubstringsCount(const std::string & str, const std::string & substr)
{
	size_t count = 0;
	for (size_t pos = str.find(substr, 0);
		pos != std::string::npos;
		pos = str.find(substr, pos + substr.size())) {
		++count;
	}
	return count;
}

