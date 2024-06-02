#include "visualizationmodel.h"
#include "../events/repaintevent.h"

QPoint VisualizationModel::MapToVisualization(QPoint model_point) const {
  return model_point + _center_coord;
}

QPoint VisualizationModel::MapToModel(QPoint vis_point) const {
  return vis_point - _center_coord;
}

QPoint VisualizationModel::GetCenterCoord() const { return _center_coord; }

void VisualizationModel::SetNewCoordCenter(QPoint new_center_coord) {
  _center_coord = new_center_coord / _mouse_sensetivity;
  Notify(std::make_shared<RepaintEvent>());
}

void VisualizationModel::SetMouseSensetivity(unsigned int mouse_sensetivity) {
  if (mouse_sensetivity > 0)
    _mouse_sensetivity = mouse_sensetivity;
}
