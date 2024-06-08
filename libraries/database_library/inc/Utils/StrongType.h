#pragma once

//------------------------------------------------------------------------------
/**
  \class StrongType
  \brief Тип данных, запрещающий неявное приведение сторонних типов при его
         конструировании.
  \tparam Type Тип данных, для которого разрешено неявное приведение
*/
//---

// Пример:
// StrongType<int> a = 5; // ok
// StrongType<int> b = 5.0; // error - попытались неявно привести double к int.
template <class Type>
class StrongType {
public:
  Type value; ///< Значение

  /// Конструктор от нужного типа данных
  StrongType(const Type &value) : value(value) {}
  /// Конструирование от других типов данных запрещено
  template <class OtherType> StrongType(const OtherType &other) = delete;
};
