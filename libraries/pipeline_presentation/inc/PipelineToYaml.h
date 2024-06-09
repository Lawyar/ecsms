#pragma once

#include "Pipeline.h"
#include "YamlPipelineStage.h"

#include <filesystem>

#include <yaml-cpp/yaml.h>

class PipelineToYaml {
 public:
  static void serializeToFile(const std::shared_ptr<Pipeline> pipeline,
                              const std::filesystem::path& file);

  static std::string serializeToString(
      const std::shared_ptr<Pipeline> pipeline);

 private:
  static YamlPipelineStage toYamlStage(
      const std::shared_ptr<IPipelineStage> stage);

  static void emitToFile(const std::list<YamlPipelineStage>& stages,
                         const std::filesystem::path& file);

  static std::string emitToString(const std::list<YamlPipelineStage>& stages);

  static std::list<YamlPipelineStage> reorderStages(
      const std::list<YamlPipelineStage>&);

  static void emit(YAML::Emitter& emitter, const std::list<YamlPipelineStage>&);
};
