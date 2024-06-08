#pragma once

#include <IExecutorEAVNamingRules.h>

//------------------------------------------------------------------------------
/**
  �����, ������������ ������� ���������� ������ ���� ������, ��������������
  ������ EAV.

  ��� ������ ����������� ���� �������� <EntityName> � ���� ������ ������������
  ��������� �������:
  1) <EntityName> - ������� ��������� - ��������:
	 "id" - ������������� ��������, �������� ��������� ������.
  2) <EntityName>_attribute_<AttributeType> - ��������� ������ ��������� - �� ����� �������
	 ��� ������� ���� ������, ������� ����� �������. ������ ������� �������� ����:
	 "id" - ������������� �������� - �������� ��������� ������.
	 "name" - �������� �������� - �� ���� �������� ����������� UNIQUE (�� ����� ���� ������ ��������� � ����������� ����������);
  3) <EntityName>_value_<AttributeType> - ������� �������� ��������� ��������� - �������� ����:
	 "entity_id" - ������������� ��������, � ������� ������������� �������� � ���� ������ - �������� ������� ������.
	 "attribute_id" - ������������� ��������, � ������� ������������� �������� � ���� ������ - �������� ������� ������.
	 ���� (entity_id, attribute_id) �������� ��������� ������.
	 "value" - �������� �������� ��� ������ ��������. ��� ���� ����� ��� <AttributeType>.
  ������ ����� ���� ������ - ����� ��� ���� ���������: User � Product.
  � User ���� ��� �������� ���� ����� - ��� (text), ����� (text) � ���� �������� (timestamp).
  � Product ���� ��� �������� ���� ����� - �������� (text) � ���� (numeric).
  ����� � ���� ������ ����� �������: user, user_attribute_text, user_attribute_timestamp, user_value_text, user_value_timestamp,
  product, product_attribute_text, product_attribute_numeric, product_value_text, product_value_numeric.
*/
//---
class PGExecutorEAVNamingRules : public IExecutorEAVNamingRules
{
public:
	/// �������� �������� ������� ���������
	virtual std::string GetEntityTableName(const std::string & entityName) const override;
	/// �������� �������� ������� ���������
	virtual std::string GetAttributeTableName(const std::string & entityName,
		const std::string & attributeType) const override;
	/// �������� �������� ������� ��������
	virtual std::string GetValueTableName(const std::string & entityName,
		const std::string & attributeType) const override;

public:
	/// �������� �������� ���� �������������� ������� ���������
	virtual std::string GetEntityTable_Short_IdField(const std::string & entityName) const override;

public:
	/// �������� �������� ���� �������������� ������� ���������
	virtual std::string GetAttributeTable_Short_IdField(const std::string & entityName,
		const std::string & attributeType) const override;

	/// �������� �������� ���� �������� ������� ���������
	virtual std::string GetAttributeTable_Short_NameField(const std::string & entityName,
		const std::string & attributeType) const override;

public:
	/// �������� �������� ���� �������������� �������� ������� ��������
	virtual std::string GetValueTable_Short_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const override;

	/// �������� �������� ���� �������������� �������� ������� ��������
	virtual std::string GetValueTable_Short_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const override;

	/// �������� �������� ���� �������� �������� ������� ��������
	virtual std::string GetValueTable_Short_ValueField(const std::string & entityName,
		const std::string & attributeType) const override;
};
