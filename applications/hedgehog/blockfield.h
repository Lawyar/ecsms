#ifndef BLOCKFIELD_H
#define BLOCKFIELD_H

#include "observer/iobserver.h"
#include "controlls/defaultcontroller.h"
#include "controlls/drawlinecontroller.h"
#include "models/activenodesmodel.h"

#include <QMap>
#include <QWidget>

class ConnectNodeWidget;

class BlockField : public QWidget, public IObserver {
  Q_OBJECT
public:
  BlockField(QWidget *parent = nullptr);
  void AddNewBlock();
  virtual void Update(std::shared_ptr<Event> e) override;
  std::unique_ptr<IController> &GetController();
  QWidget *FindById(Id id);

protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

private:
  NameMaker _block_name_maker;
  std::unique_ptr<IController> _controller;
  QPoint _pos;
  FieldModel _field_model;
  SelectionModel _selection_model;
  LineModel _line_model;
  ActiveNodesModel _active_nodes_model;
};

#endif // BLOCKFIELD_H
