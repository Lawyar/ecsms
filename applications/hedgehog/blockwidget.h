#ifndef BLOCKWIDGET_H
#define BLOCKWIDGET_H

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
  explicit BlockWidget(std::unique_ptr<IController> &controller,
                       BlockField *parent = nullptr);
  ConnectNodeWidget *GetLeftNode();
  ConnectNodeWidget *GetRightNode();

protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
  void on_pushButton_clicked();

private:
  std::unique_ptr<IController> &_controller;
  QLabel *_block_name;
  ConnectNodeWidget *_left_node, *_right_node;
  QPushButton *_resume_pause_button;
  QPoint _old_pos;
};

#endif // BLOCKWIDGET_H
