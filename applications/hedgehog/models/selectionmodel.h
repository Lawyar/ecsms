#pragma once

#include "../connectnodewidget.h"
#include "imodel.h"

#include <QMap>

class SelectionModel : public IModel {
public:
  SelectionModel() = default;
  const QMap<const ConnectNodeWidget *, QVector<const ConnectNodeWidget *>> &
  GetSelectionMap() const;
  void AddSelection(const ConnectNodeWidget *start,
                    const ConnectNodeWidget *end);
  void RemoveSelection(const ConnectNodeWidget *start,
                       const ConnectNodeWidget *end);
  void Clear();

private:
  QMap<const ConnectNodeWidget *, QVector<const ConnectNodeWidget *>>
      _map_of_selected_nodes;
};
