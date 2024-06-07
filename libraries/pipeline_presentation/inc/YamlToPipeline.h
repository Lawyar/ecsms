#pragma once

#include "Pipeline.h"
#include "PipelineStageType.h"
#include "ConsumptionStrategy.h"
#include "PipelineRegistry.h"

#include <filesystem>
#include <optional>

#include <yaml-cpp/yaml.h>

class YamlToPipeline {
 public:
  static Pipeline load(const std::filesystem::path& path);

  static Pipeline load(const std::string& input);

 private:
  struct PipelineStage {
    std::string stageName;
    std::string stageId;
    PipelineStageType stageType;
    std::optional<std::string> parentStageId;
    std::optional<ConsumptionStrategy> consumptionStrategy;
  };

  static std::vector<PipelineStage> parse(const std::vector<YAML::Node>& nodes);

  static PipelineStage parseStage(const YAML::Node& node);

  static Pipeline toPipeline(const std::vector<PipelineStage>& stages);

  static std::shared_ptr<IPipelineStage> constructProducer(
      const PipelineRegistry& registry,
      const PipelineStage& yamlStage,
      std::map<std::string, std::shared_ptr<StageConnection>>& connectionsMap);

    static std::shared_ptr<IPipelineStage> constructConsumer(
      const PipelineRegistry& registry,
      const PipelineStage& yamlStage,
      std::map<std::string, std::shared_ptr<StageConnection>>& connectionsMap);

      static std::shared_ptr<IPipelineStage> constructConsumerAndProducer(
        const PipelineRegistry& registry,
        const PipelineStage& yamlStage,
        std::map<std::string, std::shared_ptr<StageConnection>>&
            connectionsMap);

  static constexpr size_t defaultConnectionSize = 512;
};
