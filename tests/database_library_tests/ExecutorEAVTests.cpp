////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для ExecutorEAV
*/
//
////////////////////////////////////////////////////////////////////////////////

#include "TestSettings.h"

#include <gtest/gtest.h>

#include <Utils/StringUtils.h>
#include <IDatabaseManager.h>

TEST(ExecutorEAV, test)
{
	auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
	auto && executorEAV = GetDatabaseManager().GetExecutorEAV(connection);
}
