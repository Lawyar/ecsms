#pragma once

#include <IExecuteResult.h>
#include <libpq-fe.h>

//------------------------------------------------------------------------------
/**
  ����� ���������� ������� � ���� ������ PostgreSQL.
*/
//---
class PGExecuteResult : public IExecuteResult
{
	PGresult * m_result = nullptr; ///< ��������� �������
public:
	/// �����������
	PGExecuteResult(PGresult * result);
	/// ����������
	virtual ~PGExecuteResult() override;
	/// ����������� �����������
	PGExecuteResult(const PGExecuteResult &) = delete;
	/// ����������� �����������
	PGExecuteResult(PGExecuteResult &&) = delete;
	/// �������� ������������ ������������
	PGExecuteResult& operator=(const PGExecuteResult &) = delete;
	/// �������� ������������ ������������
	PGExecuteResult& operator=(PGExecuteResult &&) = delete;

public:
	/// �������� ������� ������ �������
	virtual IExecuteResultStatusPtr GetCurrentExecuteStatus() const override;

public:
	/// �������� ���������� ����� � ������
	virtual size_t GetRowCount() const override;
	/// �������� ���������� �������� � ������
	virtual size_t GetColCount() const override;
	
	/// �������� �������� ������� (���������� ������ ������, ���� ������� ���������� ������)
	virtual std::string GetColName(size_t columnIndex) const override;
	/// �������� ������ ������� �� ����� (���������� ���������� ������, ���� �������� �������������� ���)
	virtual size_t GetColIndex(const std::string & columnName) override;
	
	/// �������� ��� ������ ������� (���������� ���������� ��� ������, ���� ������� ���������� ������)
	virtual SQLDataType GetColType(size_t columnIndex) const override;

	/// �������� ��������.
	virtual CellType GetValue(size_t rowIndex, size_t columnIndex) override;
};
