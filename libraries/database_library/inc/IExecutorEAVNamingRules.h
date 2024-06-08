#pragma once

#include <string>

//------------------------------------------------------------------------------
/**
  ���������, ������������ ������� ���������� ������ ��� IExecutorEAV.
*/
//---
class IExecutorEAVNamingRules
{
public:
	/// ����������
	virtual ~IExecutorEAVNamingRules() = default;

public:
	/// �������� �������� ������� ���������
	virtual std::string GetEntityTableName(const std::string & entityName) const = 0;
	/// �������� �������� ������� ���������
	virtual std::string GetAttributeTableName(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� �������� ������� ��������
	virtual std::string GetValueTableName(const std::string & entityName,
		const std::string & attributeType) const = 0;

public:
	/// �������� �������� ���� �������������� ������� ���������
	virtual std::string GetEntityTable_Short_IdField(const std::string & entityName) const = 0;
	/// �������� ������ �������� ���� �������������� ������� ���������, ����������
	/// �������� �������
	std::string GetEntityTable_Full_IdField(const std::string & entityName) const
	{
		return GetEntityTable_Short_IdField(entityName) + "." + GetEntityTableName(entityName);
	}

public:
	/// �������� �������� ���� �������������� ������� ���������
	virtual std::string GetAttributeTable_Short_IdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� ������ �������� ���� �������������� ������� ���������, ����������
	/// �������� ������� ���������
	std::string GetAttributeTable_Full_IdField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetAttributeTableName(entityName, attributeType) + "." +
			GetAttributeTable_Short_IdField(entityName, attributeType);
	}

	/// �������� �������� ���� �������� ������� ���������
	virtual std::string GetAttributeTable_Short_NameField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� ������ �������� ���� �������� ������� ���������, ���������� ��������
	/// ������� ���������
	std::string GetAttributeTable_Full_NameField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetAttributeTableName(entityName, attributeType) + "." +
			GetAttributeTable_Short_NameField(entityName, attributeType);
	}

public:
	/// �������� �������� ���� �������������� �������� ������� ��������
	virtual std::string GetValueTable_Short_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� ������ �������� ���� �������������� �������� ������� ��������,
	/// ���������� �������� ������� ��������
	std::string GetValueTable_Full_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetValueTableName(entityName, attributeType) + "." +
			GetValueTable_Short_EntityIdField(entityName, attributeType);
	}

	/// �������� �������� ���� �������������� �������� ������� ��������
	virtual std::string GetValueTable_Short_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� ������ �������� ���� �������������� �������� ������� ��������,
	/// ���������� �������� ������� ��������
	std::string GetValueTable_Full_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetValueTableName(entityName, attributeType) + "." +
			GetValueTable_Short_AttributeIdField(entityName, attributeType);
	}

	/// �������� �������� ���� �������� �������� ������� ��������
	virtual std::string GetValueTable_Short_ValueField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� ������ �������� ���� �������� �������� ������� ��������,
	/// ���������� �������� ������� ��������
	std::string GetValueTable_Full_ValueField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetValueTableName(entityName, attributeType) + "." +
			GetValueTable_Short_ValueField(entityName, attributeType);
	}
};
