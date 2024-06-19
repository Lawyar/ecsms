#include <fstream>
#include <iostream>
#include <numeric>
#include <cmath>
#include "analytics.h"

void Analytics::calculateMean(const std::vector<unsigned char>& data) {
  double sum = 0.0;
  for (const auto& value : data) {
    sum += value;
  }
  double mean = sum / data.size();
}

void Analytics::calculateStandardDeviation(
    const std::vector<unsigned char>& data) {
  double sum = 0.0;
  for (const auto& value : data) {
    sum += value;
  }
  double mean = sum / data.size();

  double variance = 0.0;
  for (const auto& value : data) {
    variance += pow(value - mean, 2);
  }
  double stdDeviation = sqrt(variance / data.size());
}

void Analytics::calculateCorrelationCoefficient(
    const std::vector<unsigned char>& data1,
    const std::vector<unsigned char>& data2,
    double& result) {
  if (data1.size() != data2.size()) {
    std::cerr << "Размеры входных данных не совпадают." << std::endl;
    result = 0.0;  // или другое значение, указывающее на ошибку
    return;
  }

  double sum1 = 0.0;
  double sum2 = 0.0;
  double sumSquare1 = 0.0;
  double sumSquare2 = 0.0;
  double sumProduct = 0.0;

  // Вычисляем суммы элементов массивов
  for (size_t i = 0; i < data1.size(); ++i) {
    sum1 += data1[i];
    sum2 += data2[i];
    sumSquare1 += data1[i] * data1[i];
    sumSquare2 += data2[i] * data2[i];
    sumProduct += data1[i] * data2[i];
  }

  // Вычисляем коэффициент корреляции
  double n = data1.size();
  double numerator = n * sumProduct - sum1 * sum2;
  double denominator =
      sqrt((n * sumSquare1 - sum1 * sum1) * (n * sumSquare2 - sum2 * sum2));

  if (denominator == 0) {
    std::cerr << "Один или оба набора данных имеют нулевую дисперсию."
              << std::endl;
    result = 0.0;  // или другое значение, указывающее на ошибку
    return;
  }

  result = numerator / denominator;
}

ComparisonResult Analytics::compareDataByAlgorithms(
    const std::vector<unsigned char>& data1,
    const std::vector<unsigned char>& data2,
    double& simpleMeanResult,
    double& movingAverageResult) {
  double mean1_simple;
  double mean2_simple;
  calculateSimpleMean(data1, mean1_simple);
  calculateSimpleMean(data2, mean2_simple);

  double mean1_moving;
  double mean2_moving;
  calculateMovingAverage(data1, mean1_moving);
  calculateMovingAverage(data2, mean2_moving);

  simpleMeanResult = mean1_simple;
  movingAverageResult = mean1_moving;

  if (mean1_simple == mean2_simple && mean1_moving == mean2_moving) {
    return ComparisonResult::Equal;
  } else {
    return ComparisonResult::Different;
  }
}

void Analytics::calculateSimpleMean(const std::vector<unsigned char>& data,
                                    double& result) {
  if (data.empty()) {
    result = 0.0;
    return;
  }

  double sum = 0.0;
  for (const auto& value : data) {
    sum += value;
  }
  result = sum / data.size();
}

void Analytics::calculateMovingAverage(const std::vector<unsigned char>& data,
                                       double& result) {
  if (data.empty()) {
    result = 0.0;
    return;
  }

  double sum = 0.0;
  constexpr size_t window_size = 5;  // Размер окна для скользящего среднего

  // Вычисляем сумму первых window_size элементов
  for (size_t i = 0; i < window_size && i < data.size(); ++i) {
    sum += data[i];
  }

  // Вычисляем скользящее среднее для остальных элементов
  for (size_t i = window_size; i < data.size(); ++i) {
    sum -= data[i - window_size];  // Вычитаем элемент, который вышел из окна
    sum += data[i];  // Добавляем новый элемент в окно
  }

  result = sum / (data.size() - window_size +
                  1);  // Делим на количество элементов в окне
}