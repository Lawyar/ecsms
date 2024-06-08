#pragma once

#include <DataType/ISQLType.h>

#include <string>
#include <optional>

//------------------------------------------------------------------------------
/**
  ��������� SQL-���� ������ "text"
*/
//---
class ISQLTypeText : public ISQLType
{
public:
	/// ����������
	virtual ~ISQLTypeText() override = default;

public:
	/// �������� ��� ������
	virtual SQLDataType GetType() const override final { return SQLDataType::Text; }

public:
	/// �������� ��������
	virtual std::optional<std::string> GetValue() const = 0;
	/// ���������� ��������
	virtual void SetValue(const std::string & value) = 0;
};


/// ��� ��������� �� ISQLTypeText
using ISQLTypeTextPtr = std::shared_ptr<ISQLTypeText>;
