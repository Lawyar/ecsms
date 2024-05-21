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


///  Сгенерировать массив байтов
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
	size_t result = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes, &result));
	ASSERT_EQ(result, 1000);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Можно писать файл, если он открыт на дозапись
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


/// Нельзя писать в файл, если он открыт на чтение
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


/// Можно читать из файла, если он открыт на чтение
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


/// Нельзя читать из файла, если он открыт на запись
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


/// Нельзя читать из файла, если он открыт на дозапись
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


/// Режим открытия на запись прочищает файл, созданный в рамках той же транзакции
TEST_F(TestWithValidRemoteFile, WriteModeClearsFileCreatedByTheSameTransaction)
{
	constexpr size_t size = 1000;

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	{
		// Сначала что-то запишем (чтобы было что прочищать)
		auto bytes1 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
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
		std::vector<char> buffer;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		ASSERT_TRUE(buffer.empty());
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
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
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
		std::vector<char> buffer;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		ASSERT_TRUE(buffer.empty());
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
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// Откроем файл на дозапись
		bytes2 = GenerateBytes(size);
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes2, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
	}

	{
		// Проверим, что файл дополнился
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
		size_t numberOfBytesWritten = 0;
		ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
		ASSERT_EQ(numberOfBytesWritten, size);
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}

	{
		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		// Откроем файл на дозапись
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
		// Проверим, что файл дополнился
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
	size_t numberOfBytesWritten = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// Прочитаем байты
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open({ FileOpenMode::Read }));

	{
		std::vector<char> buffer;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(size, buffer));
		ASSERT_EQ(bytes, buffer);
	}
	{
		std::vector<char> buffer;
		// Проверим, что файл кончился
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1, buffer));
		ASSERT_TRUE(buffer.empty());
	}
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
	size_t numberOfBytesWritten = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);

	// Запишем ещё байты
	auto bytes2 = GenerateBytes(size);
	numberOfBytesWritten = 1;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes2, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);

	ASSERT_TRUE(remoteFilePtr->Close());

	// Прочитаем байты
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
		// Проверим, что файл кончился
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
		ASSERT_TRUE(readBytes.empty());
	}
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
	size_t numberOfBytesWritten = 0;
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes1, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);

	// Запишем ещё байты
	auto bytes2 = GenerateBytes(size);
	ASSERT_TRUE(remoteFilePtr->WriteBytes(bytes2, &numberOfBytesWritten));
	ASSERT_EQ(numberOfBytesWritten, size);

	ASSERT_TRUE(remoteFilePtr->Close());

	// Прочитаем байты
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
		// Проверим, что файл кончился
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
		ASSERT_TRUE(readBytes.empty());
	}
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


///  Сгенерировать быстро массив байтов
static std::vector<char> FastGenerateBytes(size_t count, const int * seed = nullptr)
{
	static int next = 0;
	if (seed)
		next = *seed;

	next = next * 1103515245 + 12345;

	// Дополним до границы int
	std::vector<char> arr(count % sizeof(int) == 0 ? count : count + sizeof(int) - count % sizeof(int));
	for (size_t i = 0; i < count / 4; ++i)
	{
		reinterpret_cast<int*>(arr.data())[i] = (i + 1) * next;
	}
	arr.resize(count);

	return arr;
}


/// Можно записать и прочитать большой объем данных
TEST_F(TestWithValidRemoteFile, CanWriteAndReadLargeData)
{
	// Запишем и прочитаем большой объем данных
	static constexpr size_t c_blockSize = 50'000'000ULL; ///< 50 МБ
	static constexpr size_t c_blocksCount = 4; ///< 4 блока

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));

	// Обновим семя рандома
	const int seed = 0;
	FastGenerateBytes(0, &seed);

	clock_t generateClocks = 0; ///< Время, потраченное на генерацию случайных чисел
	clock_t writeClocks = 0; ///< Время, потраченное на запись
	clock_t readClocks = 0; ///< Время, потраченное на чтение
	clock_t start = 0, end = 0;

	// Запишем байты
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

	// Прочитаем байты
	FastGenerateBytes(0, &seed); // зададим то же семя рандома, чтобы генерировать такие же массивы

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

	// Проверим, что файл кончился
	{
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
		ASSERT_TRUE(readBytes.empty());
	}
	ASSERT_TRUE(remoteFilePtr->Close());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


////////////////////////////////////////////////////////////////////////////////
// Тесты методов чтения/записи на корректность поведения при обрыве транзакций
// с пустым файлом
////////////////////////////////////////////////////////////////////////////////

/// Записанные в пустой файл данные не сохраняются при обрыве транзакции в режиме
/// открытия на запись
TEST(RemoteFile, DataWrittenToEmptyFileIsNotSavedWhenTransactionAbortsInOpenWriteMode)
{
	std::string filename;
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->CreateRemoteFile();
		filename = remoteFilePtr->GetFileName();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(GenerateBytes(1000), nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		// Обрыв транзакции
	}
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1000, readBytes));
		ASSERT_TRUE(readBytes.empty());
		ASSERT_TRUE(remoteFilePtr->Close());

		// Почистим напоследок
		ASSERT_TRUE(connection->DeleteRemoteFile(filename));
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


/// Записанные в пустой файл данные не сохраняются при откате транзакции в режиме
/// открытия на запись
TEST(RemoteFile, DataWrittenToEmptyFileIsNotSavedWhenTransactionRollbacksInOpenWriteMode)
{
	std::string filename;
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->CreateRemoteFile();
		filename = remoteFilePtr->GetFileName();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(GenerateBytes(1000), nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		// Откат транзакции
		ASSERT_FALSE(connection->RollbackTransaction()->HasError());
	}
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1000, readBytes));
		ASSERT_TRUE(readBytes.empty());
		ASSERT_TRUE(remoteFilePtr->Close());

		// Почистим напоследок
		ASSERT_TRUE(connection->DeleteRemoteFile(filename));
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


/// Записанные в пустой файл данные не сохраняются при обрыве транзакции в режиме
/// открытия на дозапись
TEST(RemoteFile, DataWrittenToEmptyFileIsNotSavedWhenTransactionAbortsInOpenAppendMode)
{
	std::string filename;
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->CreateRemoteFile();
		filename = remoteFilePtr->GetFileName();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(GenerateBytes(1000), nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		// Обрыв транзакции
	}
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1000, readBytes));
		ASSERT_TRUE(readBytes.empty());
		ASSERT_TRUE(remoteFilePtr->Close());

		// Почистим напоследок
		ASSERT_TRUE(connection->DeleteRemoteFile(filename));
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


/// Записанные в пустой файл данные не сохраняются при откате транзакции в режиме
/// открытия на дозапись
TEST(RemoteFile, DataWrittenToEmptyFileIsNotSavedWhenTransactionRollbacksInOpenAppendMode)
{
	std::string filename;
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->CreateRemoteFile();
		filename = remoteFilePtr->GetFileName();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(GenerateBytes(1000), nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		// Откат транзакции
		ASSERT_FALSE(connection->RollbackTransaction()->HasError());
	}
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		std::vector<char> readBytes;
		ASSERT_TRUE(remoteFilePtr->ReadBytes(1000, readBytes));
		ASSERT_TRUE(readBytes.empty());
		ASSERT_TRUE(remoteFilePtr->Close());

		// Почистим напоследок
		ASSERT_TRUE(connection->DeleteRemoteFile(filename));
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


////////////////////////////////////////////////////////////////////////////////
// Тесты методов чтения/записи на корректность поведения при обрыве транзакций
// с непустым файлом
////////////////////////////////////////////////////////////////////////////////

/// Записанные в непустой файл данные не сохраняются при обрыве транзакции в режиме
/// открытия на запись
TEST(RemoteFile, DataWrittenToNonEmptyFileIsNotSavedWhenTransactionAbortsInOpenWriteMode)
{
	std::string filename;
	const std::vector<char> writtenBytes = GenerateBytes(1000);
	{
		// Сначала создадим непустой файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->CreateRemoteFile();
		filename = remoteFilePtr->GetFileName();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(writtenBytes, nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
	{
		// Теперь напишем что-нибудь в этот файл
		std::vector<char> someOtherBytes = GenerateBytes(1000);

		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(someOtherBytes, nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		// Обрыв транзакции
	}
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		{
			std::vector<char> readBytes;
			ASSERT_TRUE(remoteFilePtr->ReadBytes(1000, readBytes));
			ASSERT_EQ(readBytes, writtenBytes);
		}
		{
			std::vector<char> readBytes;
			ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
			ASSERT_TRUE(readBytes.empty());
		}
		ASSERT_TRUE(remoteFilePtr->Close());

		// Почистим напоследок
		ASSERT_TRUE(connection->DeleteRemoteFile(filename));
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


/// Записанные в непустой файл данные не сохраняются при откате транзакции в режиме
/// открытия на запись
TEST(RemoteFile, DataWrittenToNonEmptyFileIsNotSavedWhenTransactionRollbacksInOpenWriteMode)
{
	std::string filename;
	const std::vector<char> writtenBytes = GenerateBytes(1000);
	{
		// Сначала создадим непустой файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->CreateRemoteFile();
		filename = remoteFilePtr->GetFileName();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(writtenBytes, nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
	{
		// Теперь напишем что-нибудь в этот файл
		std::vector<char> someOtherBytes = GenerateBytes(1000);

		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(someOtherBytes, nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		// Откат транзакции
		ASSERT_FALSE(connection->RollbackTransaction()->HasError());
	}
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		{
			std::vector<char> readBytes;
			ASSERT_TRUE(remoteFilePtr->ReadBytes(1000, readBytes));
			ASSERT_EQ(readBytes, writtenBytes);
		}
		{
			std::vector<char> readBytes;
			ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
			ASSERT_TRUE(readBytes.empty());
		}
		ASSERT_TRUE(remoteFilePtr->Close());

		// Почистим напоследок
		ASSERT_TRUE(connection->DeleteRemoteFile(filename));
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


/// Записанные в непустой файл данные не сохраняются при обрыве транзакции в режиме
/// открытия на дозапись
TEST(RemoteFile, DataWrittenToNonEmptyFileIsNotSavedWhenTransactionAbortsInOpenAppendMode)
{
	std::string filename;
	const std::vector<char> writtenBytes = GenerateBytes(1000);
	{
		// Сначала создадим непустой файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->CreateRemoteFile();
		filename = remoteFilePtr->GetFileName();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(writtenBytes, nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
	{
		// Теперь напишем что-нибудь в этот файл
		std::vector<char> someOtherBytes = GenerateBytes(1000);

		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(someOtherBytes, nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		// Обрыв транзакции
	}
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		{
			std::vector<char> readBytes;
			ASSERT_TRUE(remoteFilePtr->ReadBytes(1000, readBytes));
			ASSERT_EQ(readBytes, writtenBytes);
		}
		{
			std::vector<char> readBytes;
			ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
			ASSERT_TRUE(readBytes.empty());
		}
		ASSERT_TRUE(remoteFilePtr->Close());

		// Почистим напоследок
		ASSERT_TRUE(connection->DeleteRemoteFile(filename));
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}


/// Записанные в непустой файл данные не сохраняются при откате транзакции в режиме
/// открытия на дозапись
TEST(RemoteFile, DataWrittenToNonEmptyFileIsNotSavedWhenTransactionRollbacksInOpenAppendMode)
{
	std::string filename;
	const std::vector<char> writtenBytes = GenerateBytes(1000);
	{
		// Сначала создадим непустой файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->CreateRemoteFile();
		filename = remoteFilePtr->GetFileName();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Write));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(writtenBytes, nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
	{
		// Теперь напишем что-нибудь в этот файл
		std::vector<char> someOtherBytes = GenerateBytes(1000);

		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Append));
		ASSERT_TRUE(remoteFilePtr->WriteBytes(someOtherBytes, nullptr));
		ASSERT_TRUE(remoteFilePtr->Close());
		// Откат транзакции
		ASSERT_FALSE(connection->RollbackTransaction()->HasError());
	}
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto && remoteFilePtr = connection->GetRemoteFile(filename);

		ASSERT_FALSE(connection->BeginTransaction()->HasError());
		ASSERT_TRUE(remoteFilePtr->Open(FileOpenMode::Read));
		{
			std::vector<char> readBytes;
			ASSERT_TRUE(remoteFilePtr->ReadBytes(1000, readBytes));
			ASSERT_EQ(readBytes, writtenBytes);
		}
		{
			std::vector<char> readBytes;
			ASSERT_TRUE(remoteFilePtr->ReadBytes(1, readBytes));
			ASSERT_TRUE(readBytes.empty());
		}
		ASSERT_TRUE(remoteFilePtr->Close());

		// Почистим напоследок
		ASSERT_TRUE(connection->DeleteRemoteFile(filename));
		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}
}
