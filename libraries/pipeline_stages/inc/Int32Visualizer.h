#pragma once

#include "ConsumerStage.h"
#include "IParameterized.h"

class Int32Visualizer : public ConsumerStage<int32_t>, public IParameterized {
  std::string m_filename;

 public:
  Int32Visualizer(ConsumptionStrategy strategy,
                  std::shared_ptr<InStageConnection<int32_t>>);

  void consume(std::shared_ptr<int32_t> inData) override;

public:
  /// Получить параметры и их значения
  virtual std::vector<PatameterValue> GetPatameterValues() const override;
  /// Установить значение параметра
  virtual bool SetParameterValue(const std::string& paramName,
                                 const std::string& paramValue) override;
  /// Применить значения параметров. Возвращает ошибку через исключение.
  virtual void ApplyParameterValues() noexcept(false) override;

  /// Получить понятное название параметра
  virtual std::optional<std::wstring> GetObviousParamName(
      const std::string& paramName) const override;

  /// Объект полностью (и корректно) параметризован
  virtual bool IsFullyParameterized() const override;

 public:
  static constexpr auto stageName = "Int32Visualizer";
  static constexpr auto stageType = PipelineStageType::consumer;
  using consumptionT = int32_t;
  using productionT = void;
};
