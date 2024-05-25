#pragma once

#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

//------------------------------------------------------------------------------
/**
  Режим открытия файла
*/
//---
enum class FileOpenMode {
  Read, ///< Открыть файл на чтение. Если файл не существует, вернется ошибка.
        ///< (Поведение аналогично параметру "r" функции std::fopen)
  Write, ///< Открыть файл на запись. Если файл не существует, то будет
         ///< произведена попытка его создания. Если файл существует, то он
         ///< будет перезаписан. (Поведение аналогично параметру "w" функции
         ///< std::fopen)
  Append ///< Открыть файл на дозапись. Если файл не существует, то будет
         ///< произведена попытка его создания. Если файл существует, то данные
         ///< будут записываться в его конец. (Поведение аналогично параметру
         ///< "a" функции std::fopen)
};

//------------------------------------------------------------------------------
/**
  Интерфейс файла.
  ПРЕДУПРЕЖДЕНИЕ: Все неконстантные методы интерфейса должны вызываться в рамках
  транзакции, иначе будут возвращать признак ошибки.
*/
//---
class IFile {
public:
  virtual ~IFile() = default;

public:
  /// Получить имя файла
  virtual std::string GetFileName() const = 0;
  /// Открыть файл
  /// \param openMode Режим открытия
  /// \return Статус выполнения операции
  virtual bool Open(FileOpenMode openMode) = 0;
  /// Закрыть файл
  /// Файл, оставшийся открытым в конце транзакции, будет закрыт автоматически
  /// \return Статус выполнения операции
  virtual bool Close() = 0;

  /// Попытаться прочесть байты
  /// \param buffer Буфер, в который требуется прочитать байты.
  /// \param bytesCount Количество байт, которое требуется попытаться прочесть.
  /// \param numberOfBytesReadPtr Переменная, в которую запишется количество
  ///                             успешно прочитанных байтов. Если было
  ///                             прочитано менее, чем bytesCount байтов, это
  ///                             может свидельствовать о том, что файл
  ///                             закончился, или о том, что произошла ошибка.
  /// \return Статус выполнения операции.
  virtual bool ReadBytes(char *buffer, size_t bytesCount,
                         size_t *numberOfBytesReadPtr = nullptr) = 0;
  /// Попытаться прочесть объекты
  /// \tparam T Простой тип данных, который имеет смысл записывать в базу данных
  ///           (Например, если структура имеет указатели, которые хранят адреса
  ///           на выделенную память, то записывать эти адреса в базу данных не
  ///           имеет смысла, поскольку эти адреса в рамках другого запуска
  ///           программы будут указывать на другую область в памяти)
  /// \param elementsCount Количество элементов, которое требуется попытаться
  ///                      прочесть.
  /// \param arr Массив, в который требуется прочитать объекты.
  ///            Результат будет дописан в конец этого буфера.
  ///            Если в массив было записано менее, чем elementsCount элементов,
  ///            это может свидельствовать о том, что файл закончился, или о
  ///            том, что произошла ошибка.
  /// \param numberOfElementsReadPtr Переменная, в которую запишется количество
  ///                                успешно прочитанных байтов. Если было
  ///                                прочитано менее, чем bytesCount байтов, это
  ///                                может свидельствовать о том, что файл
  ///                                закончился, или о том, что произошла
  ///                                ошибка.
  /// \return Статус выполнения операции.
  template <class T>
  bool ReadBytes(size_t elementsCount, std::vector<T> &arr,
                 size_t *numberOfElementsReadPtr = nullptr) {
    static_assert(std::is_pod_v<T>);

    const size_t oldSize = arr.size();
    arr.resize(oldSize + elementsCount);

    size_t numberOfBytesRead = 0;
    bool result = ReadBytes(reinterpret_cast<char *>(arr.data() + oldSize),
                            elementsCount * sizeof(T), &numberOfBytesRead);

    const size_t numberOfElementsRead = numberOfBytesRead / sizeof(T);
    if (numberOfElementsReadPtr)
      *numberOfElementsReadPtr = numberOfElementsRead;

    arr.resize(oldSize + numberOfElementsRead);
    return result;
  }

  /// Попытаться записать байты
  /// \param data Массив байтов, который требуется записать.
  /// \param len Длина этого массива
  /// \param numberOfBytesWrittenPtr Переменная, в которую запишется количество
  ///                                успешно записанных байтов.
  /// \return Статус выполнения операции.
  virtual bool WriteBytes(const char *data, size_t len,
                          size_t *numberOfBytesWrittenPtr = nullptr) = 0;
  /// Попытаться записать данные.
  /// \tparam T Простой тип данных, который имеет смысл записывать в базу данных
  ///           (Например, если структура имеет указатели, которые хранят адреса
  ///           на выделенную память, то записывать эти адреса в базу данных не
  ///           имеет смысла, поскольку эти адреса в рамках другого запуска
  ///           программы будут указывать на другую область в памяти)
  /// \param data Вектор с данными.
  /// \param len Длина этого массива
  /// \param numberOfBytesWrittenPtr Переменная, в которую запишется количество
  ///                                успешно записанных байтов.
  /// \return Статус выполнения операции.
  template <class T>
  bool WriteBytes(const std::vector<T> &data,
                  size_t *numberOfElementsWrittenPtr = nullptr) {
    static_assert(std::is_pod_v<T>);

    size_t numberOfBytesWritten;
    bool result = WriteBytes(reinterpret_cast<const char *>(data.data()),
                             data.size() * sizeof(T), &numberOfBytesWritten);

    const size_t numberOfElementsWritten = numberOfBytesWritten / sizeof(T);
    if (numberOfElementsWrittenPtr)
      *numberOfElementsWrittenPtr = numberOfElementsWritten;

    return result;
  }
};

/// Указатель на IFile
using IFilePtr = std::shared_ptr<IFile>;
