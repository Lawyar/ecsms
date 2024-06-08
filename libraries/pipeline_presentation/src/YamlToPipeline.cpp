#include "YamlToPipeline.h"
#include "PipelineHelpers.h"
#include "PipelineRegistry.h"
#include "YamlToPipelineException.h"

using namespace std;
using namespace string_literals;
namespace fs = filesystem;

shared_ptr<Pipeline> YamlToPipeline::fromFile(const fs::path& path) {
  auto node = YAML::LoadFile(path.string());
  auto stages = parse(node);

  return toPipeline(stages);
}

shared_ptr<Pipeline> YamlToPipeline::fromString(const string& input) {
  auto node = YAML::Load(input);
  auto stages = parse(node);

  return toPipeline(stages);
}

vector<YamlToPipeline::PipelineStage> YamlToPipeline::parse(
    const YAML::Node& node) {
  vector<PipelineStage> stages;
  for (const auto& stageNode : node) {
    auto stage = parseStage(stageNode.second);
    stage.stageName = stageNode.first.as<string>();
    stages.push_back(stage);
  }

  return stages;
}

YamlToPipeline::PipelineStage YamlToPipeline::parseStage(
    const YAML::Node& stageNode) {
  optional<string> stageId;
  optional<PipelineStageType> stageType;
  optional<string> parentStageId;
  optional<ConsumptionStrategy> strategy;

  for (const auto& stageParameter : stageNode) {
    const auto& key = stageParameter.first.Scalar();
    auto value = stageParameter.second.as<string>();

    if (key == "id")
      stageId = value;
    else if (key == "type")
      stageType = PipelineHelpers::stagetTypeFromString(value);
    else if (key == "parentId")
      parentStageId = value;
    else if (key == "strategy")
      strategy = PipelineHelpers::strategyFromString(value);
    else
      throw YamlToPipelineException("unknown key "s + key);
  }

  if (!stageId.has_value())
    throw YamlToPipelineException(
        "yaml representation of stage doesn't contain 'id' field"s);

  if (!stageType.has_value())
    throw YamlToPipelineException(
        "yaml representation of stage doesn't contain 'type' field"s);

  return {"", stageId.value(), stageType.value(), parentStageId,
          strategy};
}

shared_ptr<Pipeline> YamlToPipeline::toPipeline(
    const vector<PipelineStage>& stages) {
  auto p = make_shared<Pipeline>();
  const auto& registry = PipelineRegistry::Instance();
  try {
    map<string, shared_ptr<StageConnection>> connections;
    for (const auto& yamlStage : stages) {
      shared_ptr<IPipelineStage> stage;

      const auto& name = yamlStage.stageName;
      const auto& id = yamlStage.stageId;
      auto type = yamlStage.stageType;
      const auto& parentId = yamlStage.parentStageId;
      auto strategy = yamlStage.consumptionStrategy;

      switch (type) {
        case PipelineStageType::producer: {
          stage = constructProducer(registry, yamlStage, connections);
          break;
        }
        case PipelineStageType::consumer: {
          stage = constructConsumer(registry, yamlStage, connections);
          break;
        }
        case PipelineStageType::producerConsumer: {
          stage =
              constructConsumerAndProducer(registry, yamlStage, connections);
          break;
        }
      }

      p->addStage(stage);
    }
    for (auto& [key, connection] : connections)
      p->addConnection(connection);
  } catch (PipelineRegistryException& ex) {
    throw YamlToPipelineException(ex.what());
  }

  return p;
}

shared_ptr<IPipelineStage> YamlToPipeline::constructProducer(
    const PipelineRegistry& registry,
    const PipelineStage& yamlStage,
    map<string, shared_ptr<StageConnection>>& connectionsMap) {
  const auto& name = yamlStage.stageName;
  const auto& id = yamlStage.stageId;
  auto type = yamlStage.stageType;
  const auto& parentId = yamlStage.parentStageId;
  auto strategy = yamlStage.consumptionStrategy;

  if (parentId.has_value())
    throw YamlToPipelineException(
        "parentId is not supported in producer stage");

  if (strategy.has_value())
    throw YamlToPipelineException(
        "strategy is not supported in producer stage");

  auto names = registry.getStageNames();
  if (find(names.begin(), names.end(), name) == names.end())
    throw YamlToPipelineException("stage with name "s + name +
                                  "was not registered in PipelineRegistry");

  auto expectedType = registry.getStageType(name);
  if (type != expectedType)
    throw YamlToPipelineException(
        "type of stage in registry differs from stage type in yaml "
        "representation");

  auto connection =
      registry.constructProducerConnection(name, defaultConnectionSize);
  auto stage = registry.constructProducer(name, connection);
  stage->setId(id);

  connectionsMap[id] = connection;

  return stage;
}

shared_ptr<IPipelineStage> YamlToPipeline::constructConsumer(
    const PipelineRegistry& registry,
    const PipelineStage& yamlStage,
    map<string, shared_ptr<StageConnection>>& connectionsMap) {
  const auto& name = yamlStage.stageName;
  const auto& id = yamlStage.stageId;
  auto type = yamlStage.stageType;
  const auto& parentId = yamlStage.parentStageId;
  auto strategy = yamlStage.consumptionStrategy;

  if (!parentId.has_value())
    throw YamlToPipelineException("consumer stage must have parentId field");

  if (!strategy.has_value())
    throw YamlToPipelineException("consumer stage must have strategy field");

  auto names = registry.getStageNames();
  if (find(names.begin(), names.end(), name) == names.end())
    throw YamlToPipelineException("stage with name "s + name +
                                  "was not registered in PipelineRegistry");

  auto expectedType = registry.getStageType(name);
  if (type != expectedType)
    throw YamlToPipelineException(
        "type of stage in registry differs from stage type in yaml "
        "representation");

  auto& parent = parentId.value();
  if (connectionsMap.find(parent) == connectionsMap.end())
    throw YamlToPipelineException(
        "error: parent stage was not declared or declared after "
        "consumer stage");
  auto connection = connectionsMap[parent];

  auto stage = registry.constructConsumer(name, strategy.value(), connection);
  stage->setId(id);
  stage->setParentId(parent);

  return stage;
}

shared_ptr<IPipelineStage> YamlToPipeline::constructConsumerAndProducer(
    const PipelineRegistry& registry,
    const PipelineStage& yamlStage,
    map<string, shared_ptr<StageConnection>>& connectionsMap) {
  const auto& name = yamlStage.stageName;
  const auto& id = yamlStage.stageId;
  auto type = yamlStage.stageType;
  const auto& parentId = yamlStage.parentStageId;
  auto strategy = yamlStage.consumptionStrategy;

  if (!parentId.has_value())
    throw YamlToPipelineException(
        "producerConsumer stage must have parentId field");

  if (!strategy.has_value())
    throw YamlToPipelineException(
        "producerConsumer stage must have strategy field");

  auto names = registry.getStageNames();
  if (find(names.begin(), names.end(), name) == names.end())
    throw YamlToPipelineException("stage with name "s + name +
                                  "was not registered in PipelineRegistry");

  auto expectedType = registry.getStageType(name);
  if (type != expectedType)
    throw YamlToPipelineException(
        "type of stage in registry differs from stage type in yaml "
        "representation");

  auto& parent = parentId.value();
  if (connectionsMap.find(parent) == connectionsMap.end())
    throw YamlToPipelineException(
        "error: parent stage was not declared or declared after "
        "consumer stage");
  auto inConnection = connectionsMap[parent];
  auto outConnection =
      registry.constructProducerConnection(name, defaultConnectionSize);

  auto stage = registry.constructConsumerAndProducer(
      name, strategy.value(), inConnection, outConnection);
  stage->setId(id);
  stage->setParentId(parent);
  connectionsMap[id] = outConnection;

  return stage;
}
