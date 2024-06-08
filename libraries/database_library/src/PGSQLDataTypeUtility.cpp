#include "PGSQLDataTypeUtility.h"

#include <server/catalog/pg_type_d.h>

#include <vector>
#include <unordered_map>


//------------------------------------------------------------------------------
/**
  ��������������� ����� ��� ����������� ���������� ����� ������ �� ������� � �������
  ��� ���������� ����� ����� ������ ��������� ������ ������ �����������,
  �������� ���� ����� ���� ������������.
*/
//---
class TypeConverter
{
private:
	/// ���������, �������� ������������ ����� ���������� � ������� ������ ������
	struct IdMatch
	{
		SQLDataType type; ///< ���������� ��� ������
		Oid id;                ///< ������� ��� ������
	};

public:
	/// ��� ���� ������������ ���������� ����� ������ � �������
	using SQLDataTypeToOidMap = std::unordered_map<SQLDataType, Oid>;
	/// ��� ���� ������������ ������� ����� ������ � ����������
	using OidToSQLDataTypeMap = std::unordered_map<Oid, SQLDataType>;

private:
	/// ������ ������������ ����� ����������� � �������� ������ ������
	const std::vector<IdMatch> m_idMatches; ///< ������ ������������ ����� ���������� � ������� ������ ������
	const SQLDataTypeToOidMap m_SQLDataTypeToOidMap; ///< ���� ������������ ���������� ����� ������ � �������
	const OidToSQLDataTypeMap m_oidToSQLDataTypeMap; ///< ���� ������������ ������� ����� ������ � ����������

public:
	/// �����������
	TypeConverter()
		: m_idMatches({
		{SQLDataType::Integer, INT4OID},
		{SQLDataType::Text, TEXTOID},
		{SQLDataType::ByteArray, BYTEAOID},
		{SQLDataType::Invalid, InvalidOid} })
		, m_SQLDataTypeToOidMap(createSQLDataTypeToOidMap(m_idMatches))
		, m_oidToSQLDataTypeMap(createOidToSQLDataTypeMap(m_idMatches))
	{}

	/// �������� ���� ������������ ���������� ����� ������ � �������
	const SQLDataTypeToOidMap & GetSQLDataTypeToOidMap() const {
		return m_SQLDataTypeToOidMap;
	}

	/// �������� ���� ������������ ������� ����� ������ � ����������
	const OidToSQLDataTypeMap & GetOidToSQLDataTypeMap() const {
		return m_oidToSQLDataTypeMap;
	}

private:
	/// ������� ���� ������������ ���������� ����� ������ � �������
	static SQLDataTypeToOidMap createSQLDataTypeToOidMap(const std::vector<IdMatch> & idMatches)
	{
		SQLDataTypeToOidMap map;
		for (auto && idMatch : idMatches)
			map.emplace(idMatch.type, idMatch.id);
		return map;
	}

	/// ������� ���� ������������ ������� ����� ������ � ����������
	static OidToSQLDataTypeMap createOidToSQLDataTypeMap(const std::vector<IdMatch> & idMatches)
	{
		OidToSQLDataTypeMap map;
		for (auto && idMatch : idMatches)
			map.emplace(idMatch.id, idMatch.type);
		return map;
	}
};


static const TypeConverter Converter; ///< ��������� ������� ����� ������ �� ���������� � �������


//------------------------------------------------------------------------------
/**
  ��������� ��� ������ � �������������, � ������� �� �������� � PostgreSQL
*/
//---
Oid SQLDataTypeToOid(SQLDataType type)
{
	auto && map = Converter.GetSQLDataTypeToOidMap();
	if (auto iter = map.find(type); iter != map.end())
		return iter->second;
	fprintf(stderr, "[Error] No match between the internal \"SQLDataType\" (%d) and the external \"Oid\" was found.",
		static_cast<int>(type));
	return InvalidOid;
}


//------------------------------------------------------------------------------
/**
  ��������� ��� ������ � �������������, � ������� �� �������� � PostgreSQL
*/
//---
SQLDataType OidToSQLDataType(Oid id)
{
	auto && map = Converter.GetOidToSQLDataTypeMap();
	if (auto iter = map.find(id); iter != map.end())
		return iter->second;
	fprintf(stderr, "[Error] No match between the external \"Oid\" (%d) and the internal \"SQLDataType\" was found.", id);
	return SQLDataType::Unknown;
}
