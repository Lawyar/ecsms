#pragma once

#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>
#include <string>
#include <cmath>

enum class ComparisonResult { 
  Equal, 
  Different 
};

class Analytics {
 public:
  static double calculateMean(const std::vector<unsigned char>& data);
  static void calculateStandardDeviation(
      const std::vector<unsigned char>& data);
  static void Analytics::calculateCorrelationCoefficient(
      const std::vector<unsigned char>& data1,
      const std::vector<unsigned char>& data2,
      double& result);
  static ComparisonResult compareDataByAlgorithms(
      const std::vector<unsigned char>& data1,
      const std::vector<unsigned char>& data2,
      double& simpleMeanResult,
      double& movingAverageResult);

 private:
  static void calculateSimpleMean(const std::vector<unsigned char>& data,
                                  double& result);
  static void calculateMovingAverage(const std::vector<unsigned char>& data,
                                     double& result);
};

#endif  // ANALYTICS_H