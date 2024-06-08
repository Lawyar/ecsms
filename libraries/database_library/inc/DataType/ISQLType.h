#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

#include <DataType/SQLDataType.h>

//------------------------------------------------------------------------------
/**
  ��������� SQL-���� ������
*/
//---
class ISQLType
{
public:
	/// ����������
	virtual ~ISQLType() = default;

public:
	/// ��������������� � ������
	virtual std::optional<std::string> ToString() const = 0;
	/// �������� �������� SQL-����
	virtual const std::string & GetTypeName() const = 0;

public:
	/// �������� ��� ������
	virtual SQLDataType GetType() const = 0;

public:
	/// ��������� �������� �� ������
	/// \return ������� �� ����������� ��������
	virtual bool ReadFrom(const std::string & value) = 0;
	/// ��������� �������� �� ������� ����
	/// \return ������� �� ����������� ��������
	virtual bool ReadFrom(const std::vector<char> & value) = 0;
};


/// ��� ��������� �� ISQLType
using ISQLTypePtr = std::shared_ptr<ISQLType>;
