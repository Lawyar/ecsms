#ifndef BLOCKWIDGET_H
#define BLOCKWIDGET_H

#include "namemaker/blockid.h"
#include "namemaker/blocknamemaker.h"

#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QWidget>

class ConnectNodeWidget;
class BlockField;
class IController;

class BlockWidget : public QWidget {
  Q_OBJECT
public:
  explicit BlockWidget(const BlockId &id, std::unique_ptr<IController> &controller,
                       BlockField *parent = nullptr);
  BlockId GetId() const;
  QWidget *FindById(Id id);
  ConnectNodeWidget *GetLeftNode();
  ConnectNodeWidget *GetRightNode();
  QPoint coordToBlockField(QPoint p) const;

protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
  void on_pushButton_clicked();

private:
  BlockId _id;
  std::unique_ptr<IController> &_controller;
  QLabel *_block_name;
  ConnectNodeWidget *_left_node, *_right_node;
  QPushButton *_resume_pause_button;
  QPoint _old_pos;
};

#endif // BLOCKWIDGET_H
