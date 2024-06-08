#pragma once

#include <IExecutorEAV.h>
#include <IConnection.h>
#include <DataType/ISQLTypeConverter.h>

//------------------------------------------------------------------------------
/**
  ���������� ����������� �������� EAV.
  todo : ��������� ����������� - ������������� �������������� ����������
  ��� ��������. ��. PostgreSQL PREPARE; libpq : PQprepare.
*/
//---
class PGExecutorEAV : public IExecutorEAV
{
	IConnectionPtr m_connection;             ///< ���������� � ��
	ISQLTypeConverterPtr m_sqlTypeConverter; ///< ��������� � SQL-����
	EAVRegisterEntries m_registerEntries;    ///< ������������������ �������� � ������ ���������

public:
	/// �����������
	PGExecutorEAV(const IConnectionPtr & connection, const ISQLTypeConverterPtr & sqlTypeConverter);

public:
	/// ����������� EAV-���������
	/// \param createTables ��������� �� �������� ������� ������� �� ������������������ ���������
	virtual IExecuteResultStatusPtr RegisterEntities(const EAVRegisterEntries & entries,
		bool createTables) override;

	/// �������� �������� ������� ���������
	virtual std::string GetEntityTableName(const std::string & entityName) const override;
	/// �������� �������� ������� ���������
	virtual std::string GetAttributeTableName(const std::string & entityName,
		const std::string & attributeType) const override;
	/// �������� �������� ������� ��������
	virtual std::string GetValueTableName(const std::string & entityName,
		const std::string & attributeType) const override;

	/// �������� ������ �������� ���� �������������� ������� ���������
	virtual std::string GetEntityTable_Full_IdField(const std::string & entityName) const override;

	/// �������� ������ �������� ���� �������������� ������� ���������
	virtual std::string GetAttributeTable_Full_IdField(const std::string & entityName,
		const std::string & attributeType) const override;
	/// �������� ������ �������� ���� �������� ������� ���������
	virtual std::string GetAttributeTable_Full_NameField(const std::string & entityName,
		const std::string & attributeType) const override;

	/// �������� ������ �������� ���� �������������� �������� ������� ��������
	virtual std::string GetValueTable_Full_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const override;
	/// �������� ������ �������� ���� �������������� �������� ������� ��������
	virtual std::string GetValueTable_Full_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const override;
	/// �������� ������ �������� ���� �������� �������� ������� ��������
	virtual std::string GetValueTable_Full_ValueField(const std::string & entityName,
		const std::string & attributeType) const override;

private:
	/// �������� ������� �������� ������� ���������
	std::string createEntityTableCommand(const std::string & entityName) const;

	/// �������� ������� �������� ������� ���������
	std::string createAttributeTableCommand(const std::string & entityName,
		const std::string & attributeType) const;

	/// �������� ������� �������� ������� ��������
	std::string createValueTableCommand(const std::string & entityName,
		const std::string & attributeType) const;

public: // ������ ��� �������� ����� ��������� � ������ ��� ������������ ���������
	/// ������� ����� �������� ���������� ����
	virtual IExecuteResultStatusPtr CreateNewEntity(const EntityName & entityName, EntityId & entityId) override;
	/// ����� ��������, � ������� ���� ��� �� ��������� ��� �������-��������
	virtual IExecuteResultStatusPtr FindEntitiesByAttrValues(const EntityName & entityName,
		const std::vector<AttrValue> & attrValues, std::vector<EntityId> & entityIds) override;

private:
	/// �������� ������� "�������� �������� � ������� ��������� � ������� ����������� �������������"
	std::string insertNewEntityReturningIdCommand(const std::string & entityName) const;
	/// �������� ����� ������� "�������� �������������� �������� �� �������� �������� � ��� ��������"
	std::optional<std::string> getEntityIdByAttrValuePartCommand(const EntityName & entityName,
		const AttrValue & attrValue) const;

public: // ������ ��� �������/���������� ������
	/// �������� �������� ��� �������� ��������
	virtual IExecuteResultStatusPtr Insert(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) override;
	/// �������� �������� ��� �������� ��������
	virtual IExecuteResultStatusPtr Update(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) override;
	/// �������� �������� ��� �������� �������� ��� ��������, ���� ������ �������� ��� �� ����
	virtual IExecuteResultStatusPtr InsertOrUpdate(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) override;

private:
	/// �������� ������� "�������� ������� � ������� ���������, ��� ��������� ������ �� ������"
	std::string insertAttributeOnConflictDoNothingCommand(const EntityName & entityName, const std::string & attributeType,
		const std::string & sqlAttrName) const;
	/// �������� ����� ������� "�������� �������� � ������� ��������"
	std::optional<std::string> insertValuePartCommand(const EntityName & entityName, EntityId entityId,
		const std::string & sqlAttrName, const ValueType & value) const;
	/// �������� ������� "�������� �������� � ������� ��������"
	std::optional<std::string> insertValueCommand(const EntityName & entityName, EntityId entityId,
		const std::string & sqlAttrName, const ValueType & value) const;
	/// �������� ������� "�������� �������� � ������� ��������, ��� ��������� ������� ����������"
	std::optional<std::string> insertValueOnConflictDoUpdateCommand(const EntityName & entityName,
		EntityId entityId, const std::string & sqlAttrName, const ValueType & value) const;
	/// �������� ������� "�������� �������� � ������� ��������"
	std::optional<std::string> updateValueCommand(const EntityName & entityName, EntityId entityId,
		const std::string & sqlAttrName, const ValueType & value) const;
	/// �������� ���������� ������� "�������� ������������� �������� �� ��� ��������"
	std::string selectAttributeIdByNameInnerCommand(const EntityName & entityName,
		const std::string & attributeType, const std::string & sqlAttrName) const;

public: // ������ ��� ��������� ������
	/// �������� �������� �������� ��������.
	/// ���������� value - ������ ���� ������ (����������������� ������������� �� ���������)
	/// ���������� ���������������� ����.
	/// ��������, ���� �� ����� �������� �������� ���������� ���������, �� ����� ��������
	/// ��������� ��������� ISQLTypeTextPtr.
	/// ��������� ������ ��������� � value.
	virtual IExecuteResultStatusPtr GetValue(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, ValueType value) override;
	/// �������� �������� ���� ��������� ��������.
	virtual IExecuteResultStatusPtr GetAttributeValues(const EntityName & entityName,
		EntityId entityId, std::vector<AttrValue> & attrValues) override;

private:
	/// �������� ������� "�������� �������� �� �������������� �������� � �������� ��������"
	std::string selectValueByEntityIdAndAttributeNameCommand(const EntityName & entityName,
		EntityId entityId, const std::string & attributeType, const std::string & sqlAttrName) const;
	/// �������� ������� "�������� �������� ��������� � �� ��������"
	std::string selectAttrValuesCommand(const EntityName & entityName, EntityId entityId,
		const std::string & attributeType) const;

	/// �������� �������� ��������� �������� �� ����������.
	IExecuteResultStatusPtr getAttributeValuesImpl(const IExecuteResultPtr & result,
		std::vector<AttrValue> & attrValues) const;

private: // ����� ��������������� ������
	/// ��������� �������.
	bool executeQuery(const std::string query, IExecuteResultPtr & result, IExecuteResultStatusPtr & status);
	/// ��������� ������ � SQL-����������
	template <class SQLConcreteType, class CppConcreteType>
	bool readIntoSQLVariable(std::string && str, SQLDataType type, CppConcreteType & value,
		IExecuteResultStatusPtr & status) const;
	/// �������� �������� �������� � �����, ��������� ��� ���������� � ������
	IExecuteResultStatusPtr getSQLAttrName(const AttrName & attrName, std::string & sqlAttrName) const;
	/// �������� �������� ���� SQL
	IExecuteResultStatusPtr getSQLTypeName(SQLDataType sqlDataType, std::string & sqlTypeName) const;
};
