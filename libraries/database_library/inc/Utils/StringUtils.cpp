#include "StringUtils.h"

#include <algorithm>

//------------------------------------------------------------------------------
/**
  ѕеревести строку в нижний регистр
*/
//---
std::string utils::string::ToLower(const std::string & sourceStr)
{
	std::string str;
	std::transform(sourceStr.begin(), sourceStr.end(), std::back_inserter(str),
		// Ёто безопасный способ использовани€ tolower
		// (https://en.cppreference.com/w/cpp/string/byte/tolower)
		[](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
	return str;
}


//------------------------------------------------------------------------------
/**
  ѕеревести строку в верхний регистр
*/
//---
std::string utils::string::ToUpper(const std::string & sourceStr)
{
	std::string str;
	std::transform(sourceStr.begin(), sourceStr.end(), std::back_inserter(str),
		// Ёто безопасный способ использовани€ toupper
		// (https://en.cppreference.com/w/cpp/string/byte/toupper)
		[](char c) { return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); });
	return str;
}


//------------------------------------------------------------------------------
/**
  —трока содержит только цифры
*/
//---
bool utils::string::HasOnlyDigits(const std::string & str)
{
	return std::all_of(str.begin(), str.end(),
		// Ёто безопасный способ использовани€ isdigit
		// (https://en.cppreference.com/w/cpp/string/byte/isdigit)
		[](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
}


//------------------------------------------------------------------------------
/**
	«амена всех подстрок
	\param str —трока, в которой производитс€ замена
	\param before ѕодстрока, которую замен€ем
	\param after ѕодстрока, на которую замен€ем
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
