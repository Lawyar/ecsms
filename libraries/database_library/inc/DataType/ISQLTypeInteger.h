#pragma once

#include <DataType/ISQLType.h>

#include <string>
#include <vector>
#include <optional>

//------------------------------------------------------------------------------
/**
  ��������� SQL-���� ������ "integer"
*/
//---
class ISQLTypeInteger : public ISQLType
{
public:
	/// ����������
	virtual ~ISQLTypeInteger() override = default;

public:
	/// �������� ��� ������
	virtual SQLDataType GetType() const override final { return SQLDataType::Integer; }

public:
	/// �������� ��������
	virtual std::optional<int> GetValue() const = 0;
	/// ���������� ��������
	virtual void SetValue(int value) = 0;
};


/// ��� ��������� �� ISQLTypeInteger
using ISQLTypeIntegerPtr = std::shared_ptr<ISQLTypeInteger>;
