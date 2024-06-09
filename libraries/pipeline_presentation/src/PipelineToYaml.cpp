#include "PipelineToYaml.h"
#include "YamlConversionException.h"
#include "PipelineHelpers.h"

#include <fstream>

using namespace std;
namespace fs = filesystem;

void PipelineToYaml::serializeToFile(const shared_ptr<Pipeline> pipeline,
                                     const fs::path& file) {
  list<YamlPipelineStage> yamlStages;
  const auto& stages = pipeline->getStages();
  for (const auto stage : stages) {
    yamlStages.push_back(toYamlStage(stage));
  }

  auto stagesReordered = reorderStages(yamlStages);
  emitToFile(stagesReordered, file);
}

string PipelineToYaml::serializeToString(const shared_ptr<Pipeline> pipeline) {
  list<YamlPipelineStage> yamlStages;
  const auto& stages = pipeline->getStages();
  for (const auto stage : stages) {
    yamlStages.push_back(toYamlStage(stage));
  }

  auto stagesReordered = reorderStages(yamlStages);
  return emitToString(stagesReordered);
}

YamlPipelineStage PipelineToYaml::toYamlStage(
    const shared_ptr<IPipelineStage> stage) {
  auto id = stage->getId();
  auto stageType = stage->getStageType();
  auto parentId = stage->getParentId();
  auto strategy = stage->getConsumptionStrategy();

  if (!id.has_value())
    throw YamlConversionException("serializable pipeline stage must have id");

  if ((stageType == PipelineStageType::consumer ||
       stageType == PipelineStageType::producerConsumer) &&
      !strategy.has_value())
    throw YamlConversionException(
        "consumer stage must have consumptionStrategy");

  if ((stageType == PipelineStageType::consumer ||
       stageType == PipelineStageType::producerConsumer) &&
      !parentId.has_value())
    throw YamlConversionException("consumer stage must have parentId");

  return {stage->getName(), id.value(), stageType, parentId, strategy};
}

list<YamlPipelineStage> PipelineToYaml::reorderStages(
    const list<YamlPipelineStage>& stages) {
  list<YamlPipelineStage> result;
  auto contains = [&result](const string& id) -> bool {
    return find_if(result.begin(), result.end(),
                   [&id](const YamlPipelineStage& stage) {
                     return stage.stageId == id;
                   }) != result.end();
  };

  for (auto stage = stages.begin(); stage != stages.end(); ++stage) {
    const auto& id = stage->stageId;
    const auto& parentId = stage->parentStageId;
    auto stageType = stage->stageType;

    if (contains(id))
      continue;

    if ((stageType == PipelineStageType::consumer ||
         stageType == PipelineStageType::producerConsumer) &&
        !contains(parentId.value())) {
      for (auto s = stage; s != stages.end(); ++s) {
        if (s->stageId == parentId) {
          if (s->stageType == PipelineStageType::consumer)
            throw YamlConversionException("parent stage is not producer");

          result.push_back(*s);
          break;
        }
      }
    }

    result.push_back(*stage);
  }

  return result;
}

void PipelineToYaml::emitToFile(const list<YamlPipelineStage>& stages,
                                const fs::path& file) {
  YAML::Emitter emitter;
  emit(emitter, stages);

  ofstream f(file);
  f << emitter.c_str();
}

string PipelineToYaml::emitToString(const list<YamlPipelineStage>& stages) {
  YAML::Emitter emitter;
  emit(emitter, stages);

  return emitter.c_str();
}

void PipelineToYaml::emit(
    YAML::Emitter& emitter, const std::list<YamlPipelineStage>& stages) {
  for (const auto& stage : stages) {
    emitter << YAML::BeginMap;
    emitter << YAML::Key << stage.stageName;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "id";
    emitter << YAML::Value << stage.stageId;
    emitter << YAML::Key << "type";
    emitter << YAML::Value << PipelineHelpers::toString(stage.stageType);
    if (stage.consumptionStrategy.has_value()) {
      emitter << YAML::Key << "strategy";
      emitter << YAML::Value
              << PipelineHelpers::toString(stage.consumptionStrategy.value());
    }
    if (stage.parentStageId.has_value()) {
      emitter << YAML::Key << "parentId";
      emitter << YAML::Value << stage.parentStageId.value();
    }
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
    emitter << YAML::Newline;
    emitter << YAML::Newline;
  }
}
