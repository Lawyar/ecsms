#include "blockwidget.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QSpacerItem>
#include <QPainter>

BlockWidget::BlockWidget(QWidget *parent)
    : QWidget(parent),
      block_name(new QLabel("block name", this)),
      label(new QLabel),
      resume_pause_button(new QPushButton("||", this))
{
    block_name->setWordWrap(true);
    block_name->setAlignment(Qt::AlignCenter);
    block_name->setFrameStyle(QFrame::Box | QFrame::Plain);
    auto new_font = block_name->font();
    new_font.setPointSize(12);
    block_name->setFont(new_font);

    auto button_layout = new QHBoxLayout();
    button_layout->addStretch(5);
    button_layout->addWidget(resume_pause_button);
    button_layout->addStretch(5);

    auto name_layout = new QHBoxLayout();
    name_layout->addWidget(block_name);
    label->setPixmap(QPixmap(10, 10));
    name_layout->addWidget(label);

    auto layout = new QVBoxLayout(this);
    layout->addLayout(button_layout);
    button_layout->setParent(layout);
    layout->addLayout(name_layout);
    name_layout->setParent(layout);
    setLayout(layout);
}

void BlockWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        oldPos = event->pos();
    }
}

void BlockWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta = event->pos() - oldPos;
    move(pos() + delta);
}

void BlockWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(label);
    painter.begin(label);
    auto radius = 5;
    painter.setBrush(QBrush(Qt::green));
    auto point = QPoint(block_name->x() + block_name->width() + radius, block_name->y() + block_name->height() / 2);
    painter.drawEllipse(point, radius, radius);
    painter.end();
}
