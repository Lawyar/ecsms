#include <iostream>
#include "analytics.h"

using namespace std;

template <typename T>
void Analytics::compareData(const std::vector<T>& data1,
                           const std::vector<T>& data2) {
  if (data1.size() != data2.size()) {
    cout << "Data sizes do not match!" << endl;
    return;
  }

  for (size_t i = 0; i < data1.size(); ++i) {
    if (data1[i] != data2[i]) {
      cout << "Data differs at index " << i << endl;
      return;
    }
  }

  cout << "Data is identical." << endl;
}

template <typename T>
void Analytics::convertData(const std::vector<T>& data) {
  // Реализация конвертации данных
}

template <typename T>
void Analytics::groupData(const std::vector<T>& data, int groupSize) {
  // Реализация группировки информации
}