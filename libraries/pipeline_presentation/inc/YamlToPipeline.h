#pragma once

#include "Pipeline.h"
#include "PipelineRegistry.h"
#include "YamlPipelineStage.h"

#include <filesystem>
#include <optional>

#include <yaml-cpp/yaml.h>

class YamlToPipeline {
 public:
  static std::shared_ptr<Pipeline> parseFromFile(const std::filesystem::path& path);

  static std::shared_ptr<Pipeline> parseFromString(const std::string& input);

 private:
  static std::vector<YamlPipelineStage> parse(const YAML::Node& node);

  static YamlPipelineStage parseStage(const YAML::Node& node);

  static std::shared_ptr<Pipeline> toPipeline(const std::vector<YamlPipelineStage>& stages);

  static std::shared_ptr<IPipelineStage> constructProducer(
      const PipelineRegistry& registry,
      const YamlPipelineStage& yamlStage,
      std::map<std::string, std::shared_ptr<StageConnection>>& connectionsMap);

    static std::shared_ptr<IPipelineStage> constructConsumer(
      const PipelineRegistry& registry,
      const YamlPipelineStage& yamlStage,
      std::map<std::string, std::shared_ptr<StageConnection>>& connectionsMap);

      static std::shared_ptr<IPipelineStage> constructConsumerAndProducer(
        const PipelineRegistry& registry,
        const YamlPipelineStage& yamlStage,
        std::map<std::string, std::shared_ptr<StageConnection>>&
            connectionsMap);

  static constexpr size_t defaultConnectionSize = 512;
};
