#ifndef CONNECTNODEWIDGET_H
#define CONNECTNODEWIDGET_H

#include "blockwidget.h"
#include "controlls/icontroller.h"

#include <QLabel>

enum NodeType { Incoming, Outgoing };

class ConnectNodeWidget : public QLabel {
  Q_OBJECT
public:
  explicit ConnectNodeWidget(std::unique_ptr<IController> &controller,
                             NodeType type, QWidget *parent = nullptr);
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  NodeType getNodeType() const;
  void makeTransparent(bool value);
  QPoint coordToParent() const;

protected:
  void mousePressEvent(QMouseEvent *event) override;

private:
  std::unique_ptr<IController> &_controller;
  NodeType _type;
};

#endif // CONNECTNODEWIDGET_H
