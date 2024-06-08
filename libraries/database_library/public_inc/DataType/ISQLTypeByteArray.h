#pragma once

#include <DataType/ISQLType.h>

#include <vector>
#include <optional>

//------------------------------------------------------------------------------
/**
  ��������� SQL-���� ������ "bytea"
*/
//---
class ISQLTypeByteArray : public ISQLType
{
public:
	/// ����������
	virtual ~ISQLTypeByteArray() override = default;

public:
	/// �������� ��� ������
	virtual SQLDataType GetType() const override final { return SQLDataType::ByteArray; }

public:
	/// �������� ��������
	virtual std::optional<std::vector<char>> GetValue() const = 0;
	/// ���������� ��������
	virtual void SetValue(const std::vector<char> & value) = 0;
};


/// ��� ��������� �� ISQLTypeByteArray
using ISQLTypeByteArrayPtr = std::shared_ptr<ISQLTypeByteArray>;
