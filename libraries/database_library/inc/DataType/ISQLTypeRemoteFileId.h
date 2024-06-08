#pragma once

#include <DataType/ISQLType.h>

//------------------------------------------------------------------------------
/**
  ��������� SQL-����������, �������������� ������������� ���������� ����� �� �������
*/
//---
class ISQLTypeRemoteFileId : public ISQLType
{
public:
	/// ����������
	virtual ~ISQLTypeRemoteFileId() override = default;

public:
	/// �������� ��� ������
	virtual SQLDataType GetType() const override final { return SQLDataType::RemoteFileId; }

public:
	/// �������� ������������� ���������� �����
	virtual const std::optional<std::string> & GetId() const = 0;
};


/// ��������� �� ISQLTypeRemoteFileId
using ISQLTypeRemoteFileIdPtr = std::shared_ptr<ISQLTypeRemoteFileId>;
