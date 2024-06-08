#pragma once

#include <IExecuteResult.h>
#include <IConnection.h>
#include <DataType/ISQLType.h>
#include <DataType/ISQLTypeText.h>

#include <cstdint>
#include <memory>
#include <vector>
#include <map>

//------------------------------------------------------------------------------
/**
  ��������� ����������� �������� EAV.
  �� ������������� ������� �������, ������� ����� ��������� ��������� ��� ������
  � ����� ������, �������������� ������ EAV.
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

  ����� RegisterEntities ����� ������� ������� �� ��������� ��������, ���� �������� � ����
  ���� createTables = true.
  ��������� ������ �� ������� �������, � ���������� �������������, ���������� ���� �������.
  ��� ���������� ����� ������ ������� ����� �������� ������.
*/
//---
class IExecutorEAV
{
public:
	/// ��� ��� �������� �������� (����������� �������� ��������).
	using EntityName = std::string;
	/// ��� ��� �������������� �������� (�������� ������������� ���������� �������� � � ������� ���������).
	using EntityId = int;
	/// ��� ��� �������� ��������.
	using AttrName = ISQLTypeTextPtr;
	/// ��� �������� ��������
	using ValueType = ISQLTypePtr;

	/// ��������� ��� �������� ����: ������� � ��� ��������
	struct AttrValue
	{
		AttrName attrName; ///< �������� ��������
		ValueType value; ///< �������� ��������
	};

public:
	/// �������� ����� � ������� ���������
	struct EntityTable
	{
		static constexpr char * c_idField = "id"; ///< �������� ���� ��� �������������� �������� � ������� ���������
	};

	/// �������� ����� � ������� ���������
	struct AttributeTable
	{
		static constexpr char * c_idField = "id"; ///< �������� ���� ��� �������������� �������� � ������� ���������
		static constexpr char * c_nameField = "name"; ///< �������� ���� ��� �������� �������� � ������� ���������
	};

	/// �������� ����� � ������� ��������
	struct ValueTable
	{
		static constexpr char * c_entityIdField = "entity_id"; ///< �������� ���� ��� �������������� �������� � ������� ��������
		static constexpr char * c_attributeIdField = "attribute_id"; ///< �������� ���� ��� �������������� �������� � ������� ��������
		static constexpr char * c_valueField = "value"; ///< �������� ���� ��� �������� � ������� ��������
	};

	/// ������ � ������� EAV
	using EAVRegisterEntries = std::map<
		EntityName, // �������� ��������
		std::vector<SQLDataType>>; // ���� ���������, ������� ��� ����� ������������

public:
	/// ����������
	virtual ~IExecutorEAV() = default;

public:
	/// ����������� EAV-���������
	/// \param createTables ��������� �� �������� ������� ������� �� ������������������ ���������
	virtual IExecuteResultStatusPtr RegisterEntities(const EAVRegisterEntries & entries,
		bool createTables) = 0;

	/// �������� �������� ������� ���������
	virtual std::string GetEntityTableName(const std::string & entityName) const = 0;
	/// �������� �������� ������� ���������
	virtual std::string GetAttributeTableName(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� �������� ������� ��������
	virtual std::string GetValueTableName(const std::string & entityName,
		const std::string & attributeType) const = 0;

	/// �������� ������ �������� ���� �������������� ������� ���������
	virtual std::string GetEntityTable_Full_IdField(const std::string & entityName) const = 0;

	/// �������� ������ �������� ���� �������������� ������� ���������
	virtual std::string GetAttributeTable_Full_IdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� ������ �������� ���� �������� ������� ���������
	virtual std::string GetAttributeTable_Full_NameField(const std::string & entityName,
		const std::string & attributeType) const = 0;

	/// �������� ������ �������� ���� �������������� �������� ������� ��������
	virtual std::string GetValueTable_Full_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� ������ �������� ���� �������������� �������� ������� ��������
	virtual std::string GetValueTable_Full_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// �������� ������ �������� ���� �������� �������� ������� ��������
	virtual std::string GetValueTable_Full_ValueField(const std::string & entityName,
		const std::string & attributeType) const = 0;

public: // ������ ��� �������� ����� ��������� � ������ ��� ������������ ���������
	/// ������� ����� �������� ���������� ����
	virtual IExecuteResultStatusPtr CreateNewEntity(const EntityName & entityName, EntityId & result) = 0;
	/// ����� ��������, � ������� ���� ��� �� ��������� ��� �������-��������
	virtual IExecuteResultStatusPtr FindEntitiesByAttrValues(const EntityName & entityName,
		const std::vector<AttrValue> & attrValues, std::vector<EntityId> & result) = 0;

public: // ������ ��� �������/���������� ������
	/// �������� �������� ��� �������� ��������
	virtual IExecuteResultStatusPtr Insert(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) = 0;
	/// �������� �������� ��� �������� ��������
	virtual IExecuteResultStatusPtr Update(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) = 0;
	/// �������� �������� ��� �������� �������� ��� ��������, ���� ������ �������� ��� �� ����
	virtual IExecuteResultStatusPtr InsertOrUpdate(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) = 0;

public: // ������ ��� ��������� ������
	/// �������� �������� �������� ��������.
	/// ���������� value - ������ ���� ������ (����������������� ������������� �� ���������)
	/// ���������� ���������������� ����.
	/// ��������, ���� �� ����� �������� �������� ���������� ���������, �� ����� ��������
	/// ��������� ��������� ISQLTypeTextPtr.
	/// ��������� ������ ��������� � value.
	virtual IExecuteResultStatusPtr GetValue(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, ValueType value) = 0;
	/// �������� �������� ���� ��������� ��������.
	virtual IExecuteResultStatusPtr GetAttributeValues(const EntityName & entityName,
		EntityId entityId, std::vector<AttrValue> & attrValues) = 0;
	/// �������� ��� ������ ��� ���� ���������
	virtual IExecuteResultPtr GetEntries(const EntityName & entityName) = 0;
};

/// ��� ��������� �� ����������� EAV-��������
using IExecutorEAVPtr = std::shared_ptr<IExecutorEAV>;
