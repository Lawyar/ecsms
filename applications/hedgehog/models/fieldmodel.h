#pragma once

#include "imodel.h"
#include "../connectnodewidget.h"

#include <QMap>

class FieldModel : public IModel {
public:
  FieldModel() = default;
  const QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>> &
  GetConnectionMap() const;
  void Remove(ConnectNodeWidget *start);
  void AddConnection(ConnectNodeWidget *start,
                     ConnectNodeWidget *end);
  void RemoveConnection(ConnectNodeWidget *start, ConnectNodeWidget *end);

  bool IsNodeUsed(ConnectNodeWidget * node) const;

private:
  QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>>
      _connection_map;
};
