#pragma once

#include <DataType/SQLDataType.h>

#include <libpq-fe.h>

/// Перевести внутренний тип данных базы данных в представление, которое
/// принимает PostgreSQL (Oid)
Oid SQLDataTypeToOid(SQLDataType type);

/// Перевести внешний тип данных базы данных PostgreSQL (Oid) во внутреннее
/// представление модуля
SQLDataType OidToSQLDataType(Oid id);
