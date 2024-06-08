////////////////////////////////////////////////////////////////////////////////
//
/**
  ����� ��� ExecutorEAVNamingRules
*/
//
////////////////////////////////////////////////////////////////////////////////

#include "TestSettings.h"

#include <gtest/gtest.h>

#include <Utils/StringUtils.h>
#include <IDatabaseManager.h>

#include <IExecutorEAVNamingRules.h>

// ���� ��� ��������
class ExecutorEAVNamingRules : public ::testing::Test
{
protected:
	IConnectionPtr connection;
	IExecutorEAVPtr executorEAV;

protected:
	// �������� � ������ �����
	virtual void SetUp() override
	{
		connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		executorEAV = GetDatabaseManager().GetExecutorEAV(connection);
	}

	// �������� ������ ��� ��������
	const IExecutorEAVNamingRules & GetRules() const
	{
		return executorEAV->GetNamingRules();
	}
};


// todo
/*
TEST_F(ExecutorEAVNamingRules, test)
{
	ASSERT_EQ(GetRules().SomeMethod(SomeArg), expectedValue);
}
*/
