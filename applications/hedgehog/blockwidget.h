#ifndef BLOCKWIDGET_H
#define BLOCKWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class ConnectNodeWidget;
class BlockField;

class BlockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BlockWidget(BlockField *parent = nullptr);

protected:
        void mousePressEvent(QMouseEvent *event) override;

        void mouseMoveEvent(QMouseEvent  *event) override;

        void leaveEvent(QEvent *event) override;

        void paintEvent(QPaintEvent* event) override;

signals:
    void start(ConnectNodeWidget* start_node);

    void end(ConnectNodeWidget* end_node);

public slots:
        void on_pushButton_clicked();

        void on_leftCircle_clicked(ConnectNodeWidget* start_node);

        void on_rightCircle_clicked(ConnectNodeWidget* end_node);

private:
    QLabel* block_name;
    ConnectNodeWidget* left_node, * right_node;
    QPushButton* resume_pause_button;
    QPoint oldPos;

};

#endif // BLOCKWIDGET_H
