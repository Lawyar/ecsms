#pragma once

#include "imodel.h"

#include <QPoint>

class VisualizationModel : public IModel {
public:
	VisualizationModel() = default;
  QPoint MapToVisualization(QPoint model_point) const;
  QPoint MapToModel(QPoint vis_point) const;
  QPoint GetCenterCoord() const;
  void SetNewCoordCenter(QPoint new_center_coord);
  void SetMouseSensetivity(unsigned int mouse_sensetivity);

private:
  unsigned int _mouse_sensetivity = 1;
  QPoint _center_coord = {0, 0};
};
