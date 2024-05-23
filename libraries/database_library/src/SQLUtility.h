#pragma once

#include <string>

/// Это ключевое слово из SQL
bool IsSQLKeyword(const std::string & str);
/// Это валидный идентификатор SQL (строка может использоваться в качестве идентификатора в SQL)
bool IsValidSQLIdentifier(const std::string & str);
