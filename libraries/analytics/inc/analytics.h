#pragma once

#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>

class Analytics {
 public:
  // Функция для сравнения данных
  template <typename T>
  void compareData(const std::vector<T>& data1, const std::vector<T>& data2);

  // Функция для конвертации данных в определенный формат
  template <typename T>
  void convertData(const std::vector<T>& data);

  // Функция для группировки информации по определенным признакам
  template <typename T>
  void groupData(const std::vector<T>& data, int groupSize);

  // Другие функции анализа данных
};

#endif  // ANALYTICS_H