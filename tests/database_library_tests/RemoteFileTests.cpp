////////////////////////////////////////////////////////////////////////////////
//
/**
  ����� ��� RemoteFile
*/
//
////////////////////////////////////////////////////////////////////////////////


#include "TestSettings.h"

#include <gtest/gtest.h>

#include <Utils/StringUtils.h>
#include <IDatabaseManager.h>

#include <cstdlib>
#include <ctime>


// ���� ��� �������� � �������� ������
class TestWithValidRemoteFile : public ::testing::Test
{
protected:
	IConnectionPtr connection;
	IFilePtr remoteFilePtr;

protected:
	// �������� � ������ �����
	virtual void SetUp() override
	{
		connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		remoteFilePtr = connection->CreateRemoteFile();
	}

	// �������� � ����� �����
	virtual void TearDown() override
	{
		ASSERT_TRUE(connection->DeleteRemoteFile(remoteFilePtr->GetFileName()));

		connection.reset();
		remoteFilePtr.reset();
	}

};


static const int _InitializeSeed = (srand(time(nullptr)), 0); ///< ������������� ������ �������


//------------------------------------------------------------------------------
/**
  ������������� ������ ������
*/
//---
static std::vector<char> GenerateBytes(size_t size)
{
	std::vector<char> arr(size);
	for (size_t i = 0; i < size; ++i)
		arr[i] = rand();
	return arr;
}

////////////////////////////////////////////////////////////////////////////////
// ����� ������� ��������
////////////////////////////////////////////////////////////////////////////////

/// ����� ������ ����, ���� �� ������ �� ������
TEST_F(TestWithValidRemoteFile, CanWriteWhenFileOpenedForWriting)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
	
	std::vector<char> bytes = GenerateBytes(1000);
	size_t result = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes, &result));
	ASSERT_EQ(result, 1000);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ����� ������ ����, ���� �� ������ �� ��������
TEST_F(TestWithValidRemoteFile, CanWriteWhenFileOpenedForAppending)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));

	std::vector<char> bytes = GenerateBytes(1000);
	size_t result = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes, &result));
	ASSERT_EQ(result, 1000);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ������ ������ � ����, ���� �� ������ �� ������
TEST_F(TestWithValidRemoteFile, CantWriteWhenFileOpenedForReading)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));

	std::vector<char> bytes = GenerateBytes(1000);
	size_t result = 0;
	ASSERT_FALSE(remoteFilePtr->WriteBytes(bytes, &result));
	ASSERT_EQ(result, 0);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ����� ������ �� �����, ���� �� ������ �� ������
TEST_F(TestWithValidRemoteFile, CanReadWhenFileOpenedForReading)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
	std::vector<char> buffer;
	ASSERT_TRUE(remoteFilePtr->ReadBytes(0, buffer));
	ASSERT_TRUE(buffer.empty());
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ������ ������ �� �����, ���� �� ������ �� ������
TEST_F(TestWithValidRemoteFile, CantReadWhenFileOpenedForWriting)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
	std::vector<char> buffer;
	ASSERT_FALSE(remoteFilePtr->ReadBytes(0, buffer));
	ASSERT_TRUE(buffer.empty());
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ������ ������ �� �����, ���� �� ������ �� ��������
TEST_F(TestWithValidRemoteFile, CantReadWhenFileOpenedForAppending)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
	std::vector<char> buffer;
	ASSERT_FALSE(remoteFilePtr->ReadBytes(0, buffer));
	ASSERT_TRUE(buffer.empty());
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ����� �������� �� ������ ��������� ����, ��������� � ������ ��� �� ����������
TEST_F(TestWithValidRemoteFile, WriteModeClearsFileCreatedByTheSameTransaction)
{
	constexpr size_t size = 1000;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	{
		// ������� ���-�� ������� (����� ���� ��� ���������)
		auto bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// ������� ���� �� ������, ����� �� �����������
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// ��������, ��� ���� �����������
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		std::vector<char> buffer;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		ASSERT_TRUE(buffer.empty());
		ASSERT_TRUE(remoteFilePtr->Close());
	}
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ����� �������� �� ������ ��������� ����, ��������� � ���������� ����������
TEST_F(TestWithValidRemoteFile, WriteModeClearsFileCreatedInPreviousTransaction)
{
	constexpr size_t size = 1000;

	{
		// ������� ���-�� ������� (����� ���� ��� ���������)
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		auto bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}

	{
		// ������� ���� �� ������, ����� �� �����������
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}


	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// ��������, ��� ���� �����������
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		std::vector<char> buffer;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		ASSERT_TRUE(buffer.empty());
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


/// ����� �������� �� �������� ���������� ����, ��������� � ������ ��� �� ����������
TEST_F(TestWithValidRemoteFile, AppendModeAppendsFileCreatedByTheSameTransaction)
{
	constexpr size_t size = 1000;
	std::vector<char> bytes1;
	std::vector<char> bytes2;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	{
		// ������� ���-�� �������
		bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// ������� ���� �� ��������
		bytes2 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes2, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// ��������, ��� ���� ����������
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		std::vector<char> buffer;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		std::vector<char> combinedBytes;
		combinedBytes.insert(combinedBytes.end(), bytes1.begin(), bytes1.end());
		combinedBytes.insert(combinedBytes.end(), bytes2.begin(), bytes2.end());
		ASSERT_EQ(buffer, combinedBytes);
		ASSERT_TRUE(remoteFilePtr->Close());
	}
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ����� �������� �� �������� ���������� ����, ��������� � ���������� ����������
TEST_F(TestWithValidRemoteFile, AppendModeAppendsFileCreatedInPreviousTransaction)
{
	constexpr size_t size = 1000;
	std::vector<char> bytes1;
	std::vector<char> bytes2;

	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// ������� ���-�� �������
		bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}

	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// ������� ���� �� ��������
		bytes2 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes2, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}

	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// ��������, ��� ���� ����������
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		std::vector<char> buffer;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		std::vector<char> combinedBytes;
		combinedBytes.insert(combinedBytes.end(), bytes1.begin(), bytes1.end());
		combinedBytes.insert(combinedBytes.end(), bytes2.begin(), bytes2.end());
		ASSERT_EQ(buffer, combinedBytes);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


////////////////////////////////////////////////////////////////////////////////
// ����� ������� ������/������
////////////////////////////////////////////////////////////////////////////////


/// ���������� � ���� ����� ����� ��������
TEST_F(TestWithValidRemoteFile, CanReadAfterWriting)
{
	constexpr size_t size = 1000;

	// ������� �����
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));

	auto bytes = GenerateBytes(size);
	size_t numberOfBytesWritten = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// ��������� �����
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open({ FileOpenMode::Read }));

	{
		std::vector<char> buffer;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		ASSERT_EQ(bytes, buffer);
	}
	{
		std::vector<char> buffer;
		// ��������, ��� ���� ��������
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1, buffer));
		ASSERT_TRUE(buffer.empty());
	}
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ����� �������� ������ � ���� � ������ ������ ������ ������ � ������ � ������ ����� ����������
/// � ������ �������� �� ������
TEST_F(TestWithValidRemoteFile, CanAddDataToFileInOneSessionInOneTransactionWithWriteMode)
{
	constexpr size_t size = 1000;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));

	// ������� �����
	auto bytes1 = GenerateBytes(size);
	size_t numberOfBytesWritten = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);

	// ������� ��� �����
	auto bytes2 = GenerateBytes(size);
	numberOfBytesWritten = 1;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes2, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);

	ASSERT_TRUE(remoteFilePtr->Close());

	// ��������� �����
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
	{
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, readBytes));
		ASSERT_EQ(bytes1, readBytes);
	}
	{
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, readBytes));
		ASSERT_EQ(bytes2, readBytes);
	}
	{
		std::vector<char> readBytes;
		// ��������, ��� ���� ��������
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
		ASSERT_TRUE(readBytes.empty());
	}
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// ����� �������� ������ � ���� � ������ ������ ������ ������ � ������ � ������ ����� ����������
/// � ������ �������� �� ��������
TEST_F(TestWithValidRemoteFile, CanAddDataToFileInOneSessionInOneTransactionWithAppendMode)
{
	constexpr size_t size = 1000;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));

	// ������� �����
	auto bytes1 = GenerateBytes(size);
	size_t numberOfBytesWritten = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);

	// ������� ��� �����
	auto bytes2 = GenerateBytes(size);
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes2, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);

	ASSERT_TRUE(remoteFilePtr->Close());

	// ��������� �����
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
	{
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, readBytes));
		ASSERT_EQ(bytes1, readBytes);
	}
	{
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, readBytes));
		ASSERT_EQ(bytes2, readBytes);
	}
	{
		std::vector<char> readBytes;
		// ��������, ��� ���� ��������
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
		ASSERT_TRUE(readBytes.empty());
	}
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


//------------------------------------------------------------------------------
/**
  ������������� ������ ������ ������
*/
//---
static std::vector<char> FastGenerateBytes(size_t count, const int * seed = nullptr)
{
	static int next = 0;
	if (seed)
		next = *seed;

	next = next * 1103515245 + 12345;

	// �������� �� ������� int
	std::vector<char> arr(count % sizeof(int) == 0 ? count : count + sizeof(int) - count % sizeof(int));
	for (size_t i = 0; i < count / 4; ++i)
	{
		reinterpret_cast<int*>(arr.data())[i] = (i + 1) * next;
	}
	arr.resize(count);

	return arr;
}


/// ����� �������� � ��������� ������� ����� ������
TEST_F(TestWithValidRemoteFile, CanWriteAndReadLargeData)
{
	// ������� � ��������� ������� ����� ������
	static constexpr size_t c_blockSize = 50'000'000ULL; ///< 50 ��
	static constexpr size_t c_blocksCount = 4; ///< 4 �����

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));

	// ������� ���� �������
	const int seed = 0;
	FastGenerateBytes(0, &seed);

	clock_t generateClocks = 0; ///< �����, ����������� �� ��������� ��������� �����
	clock_t writeClocks = 0; ///< �����, ����������� �� ������
	clock_t readClocks = 0; ///< �����, ����������� �� ������
	clock_t start = 0, end = 0;

	// ������� �����
	for (size_t blockIndex = 0; blockIndex < c_blocksCount; ++blockIndex)
	{
		start = clock();
		auto bytes = FastGenerateBytes(c_blockSize);
		end = clock();
		generateClocks += end - start;

		start = clock();
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, c_blockSize);
		end = clock();
		writeClocks += end - start;
	}
	ASSERT_TRUE(remoteFilePtr->Close());

	// ��������� �����
	FastGenerateBytes(0, &seed); // ������� �� �� ���� �������, ����� ������������ ����� �� �������

	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
	for (size_t blockIndex = 0; blockIndex < c_blocksCount; ++blockIndex)
	{
		start = clock();
		auto bytes = FastGenerateBytes(c_blockSize);
		end = clock();
		generateClocks += end - start;

		start = clock();
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(c_blockSize, readBytes));
		end = clock();
		readClocks += end - start;

		ASSERT_EQ(readBytes, bytes);
	}
	std::cout << "Time spent generating random numbers: "
		<< (float)generateClocks / CLOCKS_PER_SEC << " seconds" << std::endl;
	std::cout << "Time spent writing data: "
		<< (float)writeClocks / CLOCKS_PER_SEC << " seconds" << std::endl;
	std::cout << "Time spent reading data: "
		<< (float)readClocks / CLOCKS_PER_SEC << " seconds" << std::endl;

	// ��������, ��� ���� ��������
	{
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
		ASSERT_TRUE(readBytes.empty());
	}
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


////////////////////////////////////////////////////////////////////////////////
// ����� ������� ������/������ �� ������������ ��������� ��� ������ ����������
////////////////////////////////////////////////////////////////////////////////
