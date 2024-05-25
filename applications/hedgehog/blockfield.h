#ifndef BLOCKFIELD_H
#define BLOCKFIELD_H

#include <QMap>
#include <QWidget>
#include <memory>

#include "controlls/defaultcontroller.h"
#include "controlls/drawlinecontroller.h"
#include "observer/iobserver.h"

class ConnectNodeWidget;

class BlockField : public QWidget, public IObserver {
  Q_OBJECT
public:
  BlockField(QWidget *parent = nullptr);

  virtual void Update(std::shared_ptr<Event> e) override;

protected:
  void mouseMoveEvent(QMouseEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;

  void paintEvent(QPaintEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;

public slots:
  void on_start(ConnectNodeWidget *start);

private:
  std::unique_ptr<IController> _controller;
  QPoint _pos;
  FieldModel _field_model;
  SelectionModel _selection_model;
  LineModel _line_model;
};

#endif // BLOCKFIELD_H
