#include "PGExecutorEAVNamingRules.h"

#include <Utils/StringUtils.h>

//------------------------------------------------------------------------------
/**
  �������� �������� ������� ���������
*/
//---
std::string PGExecutorEAVNamingRules::GetEntityTableName(const std::string & entityName) const
{
	return utils::string::ToLower(entityName);
}


//------------------------------------------------------------------------------
/**
  �������� �������� ������� ����������
*/
//---
std::string PGExecutorEAVNamingRules::GetAttributeTableName(const std::string & entityName,
	const std::string & attributeType) const
{
	return utils::string::ToLower(entityName) + "_attribute_" + utils::string::ToLower(attributeType);
}


//------------------------------------------------------------------------------
/**
  �������� �������� ������� ��������
*/
//---
std::string PGExecutorEAVNamingRules::GetValueTableName(const std::string & entityName,
	const std::string & attributeType) const
{
	return utils::string::ToLower(entityName) + "_value_" + utils::string::ToLower(attributeType);
}



//------------------------------------------------------------------------------
/**
  �������� �������� ���� �������������� ������� ���������
*/
//---
std::string PGExecutorEAVNamingRules::GetEntityTable_Short_IdField(const std::string & entityName) const
{
	return "id";
}


//------------------------------------------------------------------------------
/**
  �������� �������� ���� �������������� ������� ���������
*/
//---
std::string PGExecutorEAVNamingRules::GetAttributeTable_Short_IdField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "id";
}


//------------------------------------------------------------------------------
/**
  �������� �������� ���� ������� ������� ���������
*/
//---
std::string PGExecutorEAVNamingRules::GetAttributeTable_Short_NameField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "name";
}


//------------------------------------------------------------------------------
/**
  �������� �������� ���� �������������� �������� ������� ��������
*/
//---
std::string PGExecutorEAVNamingRules::GetValueTable_Short_EntityIdField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "entity_id";
}


//------------------------------------------------------------------------------
/**
  �������� �������� ���� �������������� �������� ������� ��������
*/
//---
std::string PGExecutorEAVNamingRules::GetValueTable_Short_AttributeIdField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "attribute_id";
}


//------------------------------------------------------------------------------
/**
  �������� �������� ���� �������� ������� ��������
*/
//---
std::string PGExecutorEAVNamingRules::GetValueTable_Short_ValueField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "value";
}
