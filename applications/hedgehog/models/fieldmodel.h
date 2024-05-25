#pragma once

#include "imodel.h"
#include "../connectnodewidget.h"

#include <QMap>

class FieldModel : public IModel {
public:
  FieldModel() = default;
  const QMap<const ConnectNodeWidget *, QVector<const ConnectNodeWidget *>> &
  GetConnectionMap() const;
  void Remove(const ConnectNodeWidget *start);
  void AddConnection(const ConnectNodeWidget *start,
                     const ConnectNodeWidget *end);
  void RemoveConnection(const ConnectNodeWidget *start, const ConnectNodeWidget *end);

private:
  QMap<const ConnectNodeWidget *, QVector<const ConnectNodeWidget *>>
      _connection_map;
};
