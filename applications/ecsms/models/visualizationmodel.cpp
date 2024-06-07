#include "visualizationmodel.h"
#include "../events/visualmodelupdateevent.h"

QPoint VisualizationModel::MapToVisualization(QPoint model_point) const {
  return model_point + _center_coord;
}

QPoint VisualizationModel::MapToModel(QPoint vis_point) const {
  return vis_point - _center_coord;
}

QPoint VisualizationModel::GetCenterCoord() const { return _center_coord; }

void VisualizationModel::SetNewCoordCenter(QPoint new_center_coord) {
  _center_coord = new_center_coord;
  Notify(std::make_shared<VisualModelUpdateEvent>());
}
