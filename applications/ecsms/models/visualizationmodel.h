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

private:
  QPoint _center_coord = {0, 0};
};
