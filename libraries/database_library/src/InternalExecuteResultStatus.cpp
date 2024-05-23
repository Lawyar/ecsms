#include "InternalExecuteResultStatus.h"

//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
InternalExecuteResultStatus::InternalExecuteResultStatus(
    ResultStatus status, const std::string &errorMessage)
    : m_internalInfo{status, errorMessage} {}

//------------------------------------------------------------------------------
/**
  Получить статус
*/
//---
ResultStatus InternalExecuteResultStatus::GetStatus() const {
  return m_internalInfo.status;
}

//------------------------------------------------------------------------------
/**
  Получить сообщение об ошибке
*/
//---
std::string InternalExecuteResultStatus::GetErrorMessage() const {
  return m_internalInfo.errorMessage;
}

//------------------------------------------------------------------------------
/**
  Получить статус внутренней ошибки
*/
//---
IExecuteResultStatusPtr
InternalExecuteResultStatus::GetInternalError(const std::string &errorMessage,
                                              ResultStatus status) {
  return std::make_shared<InternalExecuteResultStatus>(status, errorMessage);
}

//------------------------------------------------------------------------------
/**
  Получить успешный статус
*/
//---
IExecuteResultStatusPtr
InternalExecuteResultStatus::GetSuccessStatus(ResultStatus status) {
  return std::make_shared<InternalExecuteResultStatus>(status, "");
}
