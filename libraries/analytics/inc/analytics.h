#pragma once

#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>

class Analytics {
 public:
  // ������� ��� ��������� ������
  template <typename T>
  void compareData(const std::vector<T>& data1, const std::vector<T>& data2);

  // ������� ��� ����������� ������ � ������������ ������
  template <typename T>
  void convertData(const std::vector<T>& data);

  // ������� ��� ����������� ���������� �� ������������ ���������
  template <typename T>
  void groupData(const std::vector<T>& data, int groupSize);

  // ������ ������� ������� ������
};

#endif  // ANALYTICS_H