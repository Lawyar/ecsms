#pragma once

//------------------------------------------------------------------------------
/**
  �����, ��� ���������� ���� ������, ������������ ������� ���������� ��� ���
  ���������������.
  ��������,
  StrongType<int> a = 5; // ok
  StrongType<int> b = 5.0; // error - ���������� ������ �������� double � int.
*/
//---
template <class Type>
struct StrongType
{
	Type value;

	StrongType(const Type & value) : value(value) {}
	template<class OtherType>
	StrongType(const OtherType & other) = delete;
};
