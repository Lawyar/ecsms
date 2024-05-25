#pragma once

#include <QObject>
#include <memory>

#include "../connectnodewidget.h"
#include "imodel.h"

class LineModel : public QObject, public IModel {
  Q_OBJECT
public:
  LineModel() = default;

  ConnectNodeWidget *GetBegin();

  void SetBegin(ConnectNodeWidget *begin);

  const QPoint GetEnd() const;

  void SetEnd(QPoint end);

public slots:
  void on_start(ConnectNodeWidget *start);

private:
  ConnectNodeWidget *_selected_node = nullptr;
  QPoint _end;
};
