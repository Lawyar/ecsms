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
	/// ��� ������ ������ �������.
	/// ������ �������� ��� ��� ������ (�������� ��� ���������), ������� ������������.
	class CellType
	{
		std::optional<std::variant<std::string, std::vector<char>, nullptr_t>> m_value; ///< ��������

	public:
		/// �����������
		CellType() : m_value() {}
		/// ����������� �� nullptr
		CellType(nullptr_t) : m_value(nullptr) {}
		/// ����������� �� ������
		CellType(const std::string & str) : m_value(str) {}

		/// ����������� �����������
		CellType(const CellType &) = delete;
		/// ����������� �����������
		CellType(CellType &&) = default;
		/// �������� ������������ ������������
		CellType& operator=(const CellType &) = delete;
		/// �������� ������������ ������������
		CellType& operator=(CellType &&) = default;



		/// ������ �������� ��������
		bool HasValue() const { return !!m_value; }
		/// ������ �������� ������
		bool HasString() const { return m_value && std::holds_alternative<std::string>(*m_value); }
		/// ������ �������� NULL
		bool HasNull() const { return m_value && std::holds_alternative<nullptr_t>(*m_value); }

		/// ������� ������, ���� ������ ������������ �������
		std::string ExtractString()
		{
			std::string result;
			if (HasString())
			{
				result = std::move(std::get<std::string>(*m_value));
				m_value = std::nullopt;
			}
			return std::move(result);
		}

	private:
		// todo: IConnection::Execute ���������� � ��������� �������
		// ������������ ������ �������� �������� ��������� - ����� ������ ���������� IConnection::Execute,
		// ������� � ������ ������ �� ������������ � ���������� � protected-������.
		// ������� � ��� ������ �������� � private-������.

		/// ����������� �� ������� ����
		CellType(const std::vector<char> & arr) : m_value(arr) {}

		/// ������ �������� �������� ������
		bool HasByteArray() const { return m_value && std::holds_alternative<std::vector<char>>(*m_value); }
		/// ������� ������ ����, ���� ������ ������������ �������� ����
		std::vector<char> ExtractByteArray()
		{
			std::vector<char> result;
			if (HasByteArray())
			{
				result = std::move(std::get<std::vector<char>>(*m_value));
				m_value = std::nullopt;
			}
			return std::move(result);
		}
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
	/// ��� (� ����������� �� ���������� SQL) �� ��������� ���������� � ������� ��������,
	/// ��� ��� ������ GetColIndex("columnname") � GetColIndex("CoLuMnNaMe") �����������,
	/// ����� ������, ���� ��� ��������� ���������.
	/// ����� �������, ���� ������ �������� ����� ���������� �������� � �������������� �������
	/// (��������, SELECT 1 AS "Number"),
	/// �� � ��� ������� ���� ���������� GetColIndex("\"Number\"") ��� ����������� ������� �������,
	/// � ������ GetColIndex("Number"), ��� GetColIndex("\"number\"") ������ ���������� ������.
	virtual size_t GetColIndex(const std::string & columnName) = 0;
	
	/// �������� ��� ������ ������� (���������� ���������� ��� ������, ���� ������� ���������� ������)
	virtual SQLDataType GetColType(size_t columnIndex) const = 0;

	/// �������� ��������.
	virtual CellType GetValue(size_t rowIndex, size_t columnIndex) = 0;
};

/// ��������� �� IExecuteResult
using IExecuteResultPtr = std::shared_ptr<IExecuteResult>;
