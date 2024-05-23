#include "PGExecuteResultStatus.h"

//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGExecuteResultStatus::PGExecuteResultStatus(const PGresult *pgResult)
    : m_externalInfo{PQresultStatus(pgResult), PQresultErrorMessage(pgResult)},
      m_internalInfo(createInternalInfo(m_externalInfo.status)) {}

//------------------------------------------------------------------------------
/**
  Получить статус
*/
//---
ResultStatus PGExecuteResultStatus::GetStatus() const {
  return m_internalInfo.status;
}

//------------------------------------------------------------------------------
/**
  Получить сообщение об ошибке, связанное с командой
  \return Сообщение об ошибке, связанное с командой,
                  или пустую строку, если ошибки не произошло.
*/
//---
std::string PGExecuteResultStatus::GetErrorMessage() const {
  std::string error;
  if (!m_internalInfo.errorMessage.empty())
    error += "[Internal Error] " + m_internalInfo.errorMessage;
  if (!m_externalInfo.errorMessage.empty())
    error += std::string(!error.empty() ? "; " : "") + "[PostgreSQL Error] " +
             m_externalInfo.errorMessage;
  return error;
}

//------------------------------------------------------------------------------
/**
  Создать внутреннее представление ошибки
*/
//---
PGExecuteResultStatus::InternalInfo
PGExecuteResultStatus::createInternalInfo(ExecStatusType pqStatus) {
  InternalInfo internalInfo{ResultStatus::Unknown, std::string()};
  switch (pqStatus) {
  case PGRES_EMPTY_QUERY: // Строка, отправленная серверу, была пустой
    internalInfo.status = ResultStatus::EmptyQuery;
    break;
  case PGRES_COMMAND_OK: // Успешное завершение команды, не возвращающей никаких
                         // данных
    internalInfo.status = ResultStatus::OkWithoutData;
    break;
  case PGRES_TUPLES_OK: // Успешное завершение команды, возвращающей данные
                        // (такой, как SELECT или SHOW)
    internalInfo.status = ResultStatus::OkWithData;
    break;
  case PGRES_COPY_OUT: // Начат перенос данных Copy Out (с сервера)
  case PGRES_COPY_IN: // Начат перенос данных Copy In (на сервер)
  case PGRES_COPY_BOTH: // Начат перенос данных Copy In/Out (на сервер и с
                        // сервера)
    internalInfo.status = ResultStatus::InProgress;
    break;
  case PGRES_NONFATAL_ERROR: // Произошла не фатальная ошибка (уведомление или
                             // предупреждение)
    internalInfo.status = ResultStatus::NonFatalError;
    break;
  case PGRES_FATAL_ERROR: // Произошла фатальная ошибка
  case PGRES_BAD_RESPONSE: // Ответ сервера не был распознан
    internalInfo.status = ResultStatus::FatalError;
    break;
  case PGRES_SINGLE_TUPLE:
    // Структура PGresult содержит только одну результирующую строку,
    // возвращённую текущей командой. Этот статус имеет место только тогда,
    // когда для данного запроса был выбран режим построчного вывода. Сейчас
    // этот режим не реализован, поэтому этот статус не должен возвращаться.
    internalInfo.errorMessage =
        "[Error] The result status corresponds to the line-by-line output "
        "mode, "
        "but the use of the line-by-line output mode is not implemented.";
    internalInfo.status = ResultStatus::FatalError;
    break;
  default:
    internalInfo.errorMessage = "[Error] Unknown request result status.";
    internalInfo.status = ResultStatus::Unknown;
    break;
  }

  return internalInfo;
}
