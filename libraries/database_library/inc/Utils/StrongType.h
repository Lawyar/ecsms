#pragma once

//------------------------------------------------------------------------------
/**
  Класс, для реализация типа данных, запрещающего неявное приведение при его
  конструировании.
  Например,
  StrongType<int> a = 5; // ok
  StrongType<int> b = 5.0; // error - попытались неявно привести double к int.
*/
//---
template <class Type> struct StrongType {
  Type value;

  StrongType(const Type &value) : value(value) {}
  template <class OtherType> StrongType(const OtherType &other) = delete;
};
