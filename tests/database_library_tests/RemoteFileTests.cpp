////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для RemoteFile
*/
//
////////////////////////////////////////////////////////////////////////////////


#include "TestSettings.h"

#include <gtest/gtest.h>

#include <Utils/StringUtils.h>
#include <IDatabaseManager.h>

#include <cstdlib>
#include <ctime>


// Тест для проверок с валидным файлом
class TestWithValidRemoteFile : public ::testing::Test
{
protected:
	IConnectionPtr connection;
	IFilePtr remoteFilePtr;

protected:
	// Действия в начале теста
	virtual void SetUp() override
	{
		connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		remoteFilePtr = connection->CreateRemoteFile();
	}

	// Действия в конце теста
	virtual void TearDown() override
	{
		ASSERT_TRUE(connection->DeleteRemoteFile(remoteFilePtr->GetFileName()));

		connection.reset();
		remoteFilePtr.reset();
	}

};


static const int _InitializeSeed = (srand(time(nullptr)), 0); ///< Инициализация семени рандома


//------------------------------------------------------------------------------
/**
  Сгенерировать массив байтов
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
// Тесты режимов открытия
////////////////////////////////////////////////////////////////////////////////

/// Можно писать файл, если он открыт на запись
TEST_F(TestWithValidRemoteFile, CanWriteWhenFileOpenedForWriting)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
	
	std::vector<char> bytes = GenerateBytes(1000);
	ASSERT_EQ(remoteFilePtr->WriteBytes(bytes), 1000);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Можно писать файл, если он открыт на дозапись
TEST_F(TestWithValidRemoteFile, CanWriteWhenFileOpenedForAppending)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));

	std::vector<char> bytes = GenerateBytes(1000);
	ASSERT_EQ(remoteFilePtr->WriteBytes(bytes), 1000);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Нельзя писать в файл, если он открыт на чтение
TEST_F(TestWithValidRemoteFile, CantWriteWhenFileOpenedForReading)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));

	std::vector<char> bytes = GenerateBytes(1000);
	ASSERT_FALSE(remoteFilePtr->WriteBytes(bytes));
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Можно читать из файла, если он открыт на чтение
TEST_F(TestWithValidRemoteFile, CanReadWhenFileOpenedForReading)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
	ASSERT_TRUE(remoteFilePtr->ReadBytes(0));
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Нельзя читать из файла, если он открыт на запись
TEST_F(TestWithValidRemoteFile, CantReadWhenFileOpenedForWriting)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
	ASSERT_FALSE(remoteFilePtr->ReadBytes(0));
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Нельзя читать из файла, если он открыт на дозапись
TEST_F(TestWithValidRemoteFile, CantReadWhenFileOpenedForAppending)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
	ASSERT_FALSE(remoteFilePtr->ReadBytes(0));
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Режим открытия на запись прочищает файл, созданный в рамках той же транзакции
TEST_F(TestWithValidRemoteFile, WriteModeClearsFileCreatedByTheSameTransaction)
{
	constexpr size_t size = 1000;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	{
		// Сначала что-то запишем (чтобы было что прочищать)
		auto bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1));
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// Откроем файл на запись, чтобы он прочистился
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// Проверим, что файл прочистился
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size)->empty());
		ASSERT_TRUE(remoteFilePtr->Close());
	}
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Режим открытия на запись прочищает файл, созданный в предыдущей транзакции
TEST_F(TestWithValidRemoteFile, WriteModeClearsFileCreatedInPreviousTransaction)
{
	constexpr size_t size = 1000;

	{
		// Сначала что-то запишем (чтобы было что прочищать)
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		auto bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_EQ(remoteFilePtr->WriteBytes(bytes1), size);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}

	{
		// Откроем файл на запись, чтобы он прочистился
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}


	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// Проверим, что файл прочистился
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size)->empty());
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


/// Режим открытия на дозапись дописывает файл, созданный в рамках той же транзакции
TEST_F(TestWithValidRemoteFile, AppendModeAppendsFileCreatedByTheSameTransaction)
{
	constexpr size_t size = 1000;
	std::vector<char> bytes1;
	std::vector<char> bytes2;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	{
		// Сначала что-то запишем
		bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_EQ(remoteFilePtr->WriteBytes(bytes1), size);
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// Откроем файл на дозапись
		bytes2 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		ASSERT_EQ(remoteFilePtr->WriteBytes(bytes2), size);
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// Проверим, что файл дополнился
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		ASSERT_EQ(remoteFilePtr->ReadBytes(size), bytes1);
		ASSERT_EQ(remoteFilePtr->ReadBytes(size), bytes2);
		ASSERT_TRUE(remoteFilePtr->Close());
	}
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Режим открытия на дозапись дописывает файл, созданный в предыдущей транзакции
TEST_F(TestWithValidRemoteFile, AppendModeAppendsFileCreatedInPreviousTransaction)
{
	constexpr size_t size = 1000;
	std::vector<char> bytes1;
	std::vector<char> bytes2;

	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// Сначала что-то запишем
		bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_EQ(remoteFilePtr->WriteBytes(bytes1), size);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}

	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// Откроем файл на дозапись
		bytes2 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		ASSERT_EQ(remoteFilePtr->WriteBytes(bytes2), size);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}

	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// Проверим, что файл дополнился
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		ASSERT_EQ(remoteFilePtr->ReadBytes(size), bytes1);
		ASSERT_EQ(remoteFilePtr->ReadBytes(size), bytes2);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


////////////////////////////////////////////////////////////////////////////////
// Тесты методов чтения/записи
////////////////////////////////////////////////////////////////////////////////


/// Записанные в файл байты можно прочесть
TEST_F(TestWithValidRemoteFile, CanReadAfterWriting)
{
	constexpr size_t size = 1000;

	// Запишем байты
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));

	auto bytes = GenerateBytes(size);
	ASSERT_EQ(remoteFilePtr->WriteBytes(bytes), size);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// Прочитаем байты
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open({ FileOpenMode::Read }));

	auto readBytes = remoteFilePtr->ReadBytes(size);
	ASSERT_EQ(bytes, readBytes);
	// Проверим, что файл кончился
	ASSERT_TRUE(remoteFilePtr->ReadBytes(1)->empty());
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Можно дописать данные в файл в рамках одного сеанса работы с файлом в рамках одной транзакции
/// в режиме открытия на запись
TEST_F(TestWithValidRemoteFile, CanAddDataToFileInOneSessionInOneTransactionWithWriteMode)
{
	constexpr size_t size = 1000;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));

	// Запишем байты
	auto bytes1 = GenerateBytes(size);
	ASSERT_EQ(remoteFilePtr->WriteBytes(bytes1), size);

	// Запишем ещё байты
	auto bytes2 = GenerateBytes(size);
	ASSERT_EQ(remoteFilePtr->WriteBytes(bytes2), size);

	ASSERT_TRUE(remoteFilePtr->Close());

	// Прочитаем байты
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
	auto readBytes = remoteFilePtr->ReadBytes(size);
	ASSERT_EQ(bytes1, readBytes);
	readBytes = remoteFilePtr->ReadBytes(size);
	ASSERT_EQ(bytes2, readBytes);
	// Проверим, что файл кончился
	ASSERT_TRUE(remoteFilePtr->ReadBytes(1)->empty());
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Можно дописать данные в файл в рамках одного сеанса работы с файлом в рамках одной транзакции
/// в режиме открытия на дозапись
TEST_F(TestWithValidRemoteFile, CanAddDataToFileInOneSessionInOneTransactionWithAppendMode)
{
	constexpr size_t size = 1000;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));

	// Запишем байты
	auto bytes1 = GenerateBytes(size);
	ASSERT_EQ(remoteFilePtr->WriteBytes(bytes1), size);

	// Запишем ещё байты
	auto bytes2 = GenerateBytes(size);
	ASSERT_EQ(remoteFilePtr->WriteBytes(bytes2), size);

	ASSERT_TRUE(remoteFilePtr->Close());

	// Прочитаем байты
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
	auto readBytes = remoteFilePtr->ReadBytes(size);
	ASSERT_EQ(bytes1, readBytes);
	readBytes = remoteFilePtr->ReadBytes(size);
	ASSERT_EQ(bytes2, readBytes);
	// Проверим, что файл кончился
	ASSERT_TRUE(remoteFilePtr->ReadBytes(1)->empty());
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Можно записать и прочитать большой объем данных
TEST_F(TestWithValidRemoteFile, CanWriteAndReadLargeData)
{
	// Запишем и прочитаем большой объем данных
	static constexpr size_t c_blockSize = 200'000'000ULL; ///< 200 МБ
	static constexpr size_t c_blocksCount = 4; ///< 4 блока

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));

	// Обновим семя рандома
	time_t currentTime = time(nullptr);
	srand(currentTime);

	clock_t generateClocks = 0; ///< Время, потраченное на генерацию случайных чисел
	clock_t writeClocks = 0; ///< Время, потраченное на запись
	clock_t readClocks = 0; ///< Время, потраченное на чтение
	clock_t start = 0, end = 0;

	// Запишем байты
	for (size_t blockIndex = 0; blockIndex < c_blocksCount; ++blockIndex)
	{
		start = clock();
		auto bytes = GenerateBytes(c_blockSize);
		end = clock();
		generateClocks += end - start;

		start = clock();
		ASSERT_EQ(remoteFilePtr->WriteBytes(bytes), c_blockSize);
		end = clock();
		writeClocks += end - start;
	}
	ASSERT_TRUE(remoteFilePtr->Close());

	// Прочитаем байты
	srand(currentTime); // зададим то же семя рандома, чтобы генерировать такие же массивы

	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
	for (size_t blockIndex = 0; blockIndex < c_blocksCount; ++blockIndex)
	{
		start = clock();
		auto bytes = GenerateBytes(c_blockSize);
		end = clock();
		generateClocks += end - start;

		start = clock();
		auto readBytes = remoteFilePtr->ReadBytes(c_blockSize);
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

	// Проверим, что файл кончился
	ASSERT_TRUE(remoteFilePtr->ReadBytes(1)->empty());
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


////////////////////////////////////////////////////////////////////////////////
// Тесты методов чтения/записи на корректность поведения при обрыве транзакций
////////////////////////////////////////////////////////////////////////////////
