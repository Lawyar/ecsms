#pragma once

#include <DataType/SQLDataType.h>

#include <libpq-fe.h>

/// ��������� ���������� ��� ������ ���� ������ � �������������, ������� ��������� PostgreSQL (Oid)
Oid SQLDataTypeToOid(SQLDataType type);

/// ��������� ������� ��� ������ ���� ������ PostgreSQL (Oid) �� ���������� ������������� ������
SQLDataType OidToSQLDataType(Oid id);
