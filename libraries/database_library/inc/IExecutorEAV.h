#pragma once

#include <IExecuteResult.h>
#include <IConnection.h>
#include <DataType/ISQLType.h>
#include <DataType/ISQLTypeText.h>

#include <memory>
#include <vector>
#include <map>

class IExecutorEAVNamingRules;

//------------------------------------------------------------------------------
/**
  ��������� ����������� �������� EAV.
  �� ������������� ������� �������, ������� ����� ��������� ��������� ��� ������
  � ����� ������, �������������� ������ EAV.

  ������� ���������� ������ ��� ���� ������ ������������ ����������� IExecutorEAVNamingRules.

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
	/// �������� ������������������ ��������
	virtual const EAVRegisterEntries & GetRegisteredEntities() const = 0;

	/// �������� ������, ������������ ������� ���������� ������
	virtual const IExecutorEAVNamingRules & GetNamingRules() const = 0;

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
};

/// ��� ��������� �� ����������� EAV-��������
using IExecutorEAVPtr = std::shared_ptr<IExecutorEAV>;
