#pragma once

#include "imodel.h"
#include "yaml-cpp/yaml.h"

#include <QPoint>

class VisualizationModel : public IModel {
 public:
  VisualizationModel() = default;

  class Memento {
   public:
    Memento(const Memento&) = delete;
    Memento(Memento&&) = default;

    Memento& operator=(const Memento&) = delete;
    Memento& operator=(Memento&&) = delete;

   private:
    Memento(QPoint center_coord) : _center_coord(center_coord) {}
    QPoint _center_coord;
    friend VisualizationModel;
  };

  VisualizationModel& operator=(const VisualizationModel& other);

  QPoint MapToVisualization(QPoint model_point) const;
  QPoint MapToModel(QPoint vis_point) const;
  QPoint GetCenterCoord() const;
  void SetNewCoordCenter(QPoint new_center_coord);

  void Clear();

  Memento Save() const;
  void Load(const Memento& m);

  friend struct YAML::convert<VisualizationModel>;

 private:
  QPoint _center_coord = {0, 0};
};

namespace YAML {
template <>
struct convert<::VisualizationModel> {
  static Node encode(const ::VisualizationModel& v) {
    Node center_coord_node;
    center_coord_node["center_coord"].push_back(v._center_coord.x());
    center_coord_node["center_coord"].push_back(v._center_coord.y());
    return center_coord_node;
  }

  static bool decode(const Node& node, ::VisualizationModel& v) {
    v._center_coord.setX(node["center_coord"][0].as<int>());
    v._center_coord.setY(node["center_coord"][1].as<int>());
    return true;
  }
};
}  // namespace YAML
