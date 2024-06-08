#include "StringUtils.h"

#include <algorithm>
#include <cctype>

//------------------------------------------------------------------------------
/**
  ��������� ������ � ������ �������
*/
//---
std::string utils::string::ToLower(const std::string & sourceStr)
{
	std::string str;
	std::transform(sourceStr.begin(), sourceStr.end(), std::back_inserter(str),
		// ��� ���������� ������ ������������� tolower
		// (https://en.cppreference.com/w/cpp/string/byte/tolower)
		[](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
	return str;
}


//------------------------------------------------------------------------------
/**
  ��������� ������ � ������� �������
*/
//---
std::string utils::string::ToUpper(const std::string & sourceStr)
{
	std::string str;
	std::transform(sourceStr.begin(), sourceStr.end(), std::back_inserter(str),
		// ��� ���������� ������ ������������� toupper
		// (https://en.cppreference.com/w/cpp/string/byte/toupper)
		[](char c) { return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); });
	return str;
}


//------------------------------------------------------------------------------
/**
	������ ���� ��������
	\param str ������, � ������� ������������ ������
	\param before ���������, ������� ��������
	\param after ���������, �� ������� ��������
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
