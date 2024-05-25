#pragma once

#include "../connectnodewidget.h"
#include "imodel.h"

#include <QMap>

class SelectionModel : public IModel {
public:
  SelectionModel() = default;
  const QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>> &
  GetSelectionMap() const;
  void AddSelection(ConnectNodeWidget *start,
                    ConnectNodeWidget *end);
  void RemoveSelection(ConnectNodeWidget *start,
                       ConnectNodeWidget *end);
  void Clear();

private:
  QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>>
      _map_of_selected_nodes;
};
