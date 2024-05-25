#ifndef BLOCKWIDGET_H
#define BLOCKWIDGET_H

#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QWidget>

class ConnectNodeWidget;
class BlockField;

class BlockWidget : public QWidget {
  Q_OBJECT
public:
  explicit BlockWidget(BlockField *parent = nullptr);
  ConnectNodeWidget *GetLeftNode();
  ConnectNodeWidget *GetRightNode();

protected:
  void mousePressEvent(QMouseEvent *event) override;

  void mouseMoveEvent(QMouseEvent *event) override;

  void leaveEvent(QEvent *event) override;

  void paintEvent(QPaintEvent *event) override;

signals:
  void start(ConnectNodeWidget *start_node);

public slots:
  void on_pushButton_clicked();

  void on_leftCircle_clicked(ConnectNodeWidget *start_node);

  void on_rightCircle_clicked(ConnectNodeWidget *end_node);

private:
  QLabel *_block_name;
  ConnectNodeWidget *_left_node, *_right_node;
  QPushButton *_resume_pause_button;
  QPoint _old_pos;
};

#endif // BLOCKWIDGET_H
