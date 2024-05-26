#pragma once

#include "../../connectnodewidget.h"
#include "../../models/fieldmodel.h"
#include "icommand.h"

#include <QMap>

class DeleteBlocksAndConnectionsCommand : public ICommand {
public:
  DeleteBlocksAndConnectionsCommand(
      const QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>>
          &connection_map,
      const QSet<BlockWidget *> &blocks, FieldModel &field_model);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  void saveData();

private:
  QMap<ConnectNodeWidget *, QVector<ConnectNodeWidget *>> _connection_map;
  QSet<BlockWidget *> _blocks;
  FieldModel &_field_model;

  QMap<int, BlockWidget *> blocks_ids;
};
