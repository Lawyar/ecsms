#pragma once

#include <DataType/SQLDataType.h>
#include <IExecuteResultStatus.h>

#include <optional>
#include <string>
#include <variant>
#include <vector>

//------------------------------------------------------------------------------
/**
  ������ ������ � ����������
*/
//---
enum class ResultFormat
{
	Text,   ///< ������ ����������� � ���� ��������� ������
	Binary, ///< ������ ����������� � ���� �������� ������
	Invalid ///< ���������� ������ ������
};

//------------------------------------------------------------------------------
/**
  ��������� ���������� ������� � ���� ������.
  �� ���� ������� ���������� ���������� � ����.
*/
//---
class IExecuteResult
{
public:
	/// ��� ������ ������ �������. ���� ������ �������� null ��� �����������, �� �������� ������ - std::nullopt,
	/// � ��������� ������ ������ �������� ��������� ��� �������� ������
	/// (� ����������� �� ����, ����� ��� ������ ������������ � �������).
	// todo : ������� optional<...> ����� ������
	struct CellType : public std::optional<std::variant<std::string, std::vector<char>>>
	{
		using std::optional<std::variant<std::string, std::vector<char>>>::operator=;
		/// ������ �������� ������
		bool HasString() const { return has_value() && std::holds_alternative<std::string>(value()); }
		/// ������ �������� �������� ������
		bool HasByteArray() const { return has_value() && std::holds_alternative<std::vector<char>>(value()); }
		/// ������ �������� ������ �������� ��� ������ �����������
		bool HasNull() const { return !has_value(); }

		/// �������� ������, ���� ������ ������������ �������
		std::string GetString() const { return HasString() ? std::get<std::string>(value()) : std::string(); }
		/// �������� ������ ����, ���� ������ ������������ �������� ����
		std::vector<char> GetByteArray() const { return HasByteArray() ? std::get<std::vector<char>>(value()) : std::vector<char>(); }
	};

public:
	static constexpr size_t InvalidIndex = (size_t)(-1); ///< ���������� ������
public:
	/// ����������
	virtual ~IExecuteResult() = default;

public:
	/// �������� ������� ������ �������
	virtual IExecuteResultStatusPtr GetCurrentExecuteStatus() const = 0;

public:
	/// �������� ���������� ����� � ������
	virtual size_t GetRowCount() const = 0;
	/// �������� ���������� �������� � ������
	virtual size_t GetColCount() const = 0;
	
	/// �������� �������� ������� (���������� ������ ������, ���� ������� ���������� ������)
	virtual std::string GetColName(size_t columnIndex) const = 0;
	/// �������� ������ ������� �� ����� (���������� ���������� ������, ���� �������� �������������� ���)
	virtual size_t GetColIndex(const std::string & columnName) = 0;
	
	/// �������� ��� ������ ������� (���������� ���������� ��� ������, ���� ������� ���������� ������)
	virtual SQLDataType GetColType(size_t columnIndex) const = 0;

	/// �������� ��������.
	/// ���������� std::nullopt, ���� ������ �������� null ��� ���� �������� ���������� �������.
	/// � ��������� ������ ���������� ����� ��� �������� ������
	/// (� ����������� �� ����, � ����� ���� ������������� ������ � �������)
	virtual CellType GetValue(size_t rowIndex, size_t columnIndex) = 0;
};

/// ��������� �� IExecuteResult
using IExecuteResultPtr = std::shared_ptr<IExecuteResult>;
