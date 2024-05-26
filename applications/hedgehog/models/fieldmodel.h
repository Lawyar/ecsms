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
  void AddBlock(BlockWidget *block);
  void RemoveBlock(BlockWidget *block);

private:
  QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>>
      _connection_map;
  QSet<BlockWidget *> _blocks;
};
