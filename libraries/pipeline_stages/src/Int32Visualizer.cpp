#include "Int32Visualizer.h"

#include <fstream>
#include <iostream>

using namespace std;

Int32Visualizer::Int32Visualizer(
    ConsumptionStrategy strategy,
    std::shared_ptr<InStageConnection<int32_t>> connection)
    : ConsumerStage(stageName, strategy, connection) {}

void Int32Visualizer::consume(std::shared_ptr<int32_t> inData) {
  std::ofstream file(m_filename, std::ios::app);
  file << *inData << ' ' << endl;
  cout << "Int32Visualizer: " << *inData << endl;
  dataConsumed(inData);
}

std::vector<IParameterized::PatameterValue>
Int32Visualizer::GetPatameterValues() const {
  return std::vector<PatameterValue>({{"filename", m_filename}});
}

bool Int32Visualizer::SetParameterValue(const std::string& paramName,
                                        const std::string& paramValue) {
  bool res = paramName == "filename";
  if (res)
    m_filename = paramValue;
  return res;
}

void Int32Visualizer::ApplyParameterValues() noexcept(false) 
{
  if (!IsFullyParameterized())
    throw std::runtime_error("Can't open file");
}

std::optional<std::wstring> Int32Visualizer::GetObviousParamName(
    const std::string& paramName) const {
  if (paramName == "filename")
    return L"Название файла";
  return std::nullopt;
}

bool Int32Visualizer::IsFullyParameterized() const {
  std::ofstream file(m_filename, std::ios::app);
  return file.is_open();
}
