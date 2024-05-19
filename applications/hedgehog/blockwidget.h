#ifndef BLOCKWIDGET_H
#define BLOCKWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class BlockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BlockWidget(QWidget *parent = nullptr);

protected:
        void mousePressEvent(QMouseEvent *event) override;

        void mouseMoveEvent(QMouseEvent *event)  override;

        void paintEvent(QPaintEvent* event) override;

signals:

private:
    QLabel* block_name, * label;
    QPushButton* resume_pause_button;
    QPoint oldPos;

};

#endif // BLOCKWIDGET_H
