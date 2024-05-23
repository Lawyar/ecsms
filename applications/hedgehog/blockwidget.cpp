#include "blockwidget.h"
#include "connectnodewidget.h"
#include "blockfield.h"

#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

BlockWidget::BlockWidget(BlockField *parent)
    : QWidget(parent),
      block_name(new QLabel("block name", this)),
      left_node(new ConnectNodeWidget(Incoming, this)),
      right_node(new ConnectNodeWidget(Outgoing, this)),
      resume_pause_button(new QPushButton("||", this))
{
    qDebug() << "BlockWidget::BlockWidget";
    connect(left_node, &ConnectNodeWidget::start, this, &BlockWidget::on_leftCircle_clicked);
    connect(right_node, &ConnectNodeWidget::start, this, &BlockWidget::on_rightCircle_clicked);

    connect(this, &BlockWidget::start, parent, &BlockField::on_start);

    /*connect(left_node, &ConnectNodeWidget::end, this, &BlockWidget::on_leftCircle_clicked);
    connect(right_node, &ConnectNodeWidget::end, this, &BlockWidget::on_rightCircle_clicked);*/

    block_name->setWordWrap(true);
    block_name->setAlignment(Qt::AlignCenter);
    block_name->setFrameStyle(QFrame::Box | QFrame::Plain);
    auto new_font = block_name->font();
    new_font.setPointSize(12);
    block_name->setFont(new_font);
    resume_pause_button->setFont(new_font);

    block_name->show();
    resume_pause_button->show();

    auto label_width = block_name->width();
    auto label_height = block_name->height();
    auto spacing = 2, spacing2 = 3;
    auto button_width = resume_pause_button->width();
    auto button_height = resume_pause_button->height();
    auto radius = 5, diameter = radius * 2;

    auto width = 2 * (2 * spacing + diameter) + label_width;
    auto height = 2 * spacing + button_height + spacing2 + label_height;
    setGeometry(0,0, width, height);

    resume_pause_button->move((width - button_width) / 2, spacing);
    block_name->move(2 * spacing + diameter, spacing + button_height + spacing2);

    left_node->move(spacing, spacing + button_height + spacing2 + label_height / 2 - diameter / 2);
    right_node->move(width - spacing - diameter, spacing + button_height + spacing2 + label_height / 2 - diameter / 2);

    setMouseTracking(true);

    connect(resume_pause_button, &QPushButton::clicked, this, &BlockWidget::on_pushButton_clicked);
}

void BlockWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        oldPos = event->pos();
        parentWidget()->repaint();
    }
}

void BlockWidget::mouseMoveEvent(QMouseEvent *event)
{
    left_node->makeTransparent(false);
    right_node->makeTransparent(false);
    if (event->buttons() ==  Qt::LeftButton)
    {
        QPoint delta = event->pos() - oldPos;
        move(pos() + delta);
        parentWidget()->repaint();
    }
}

void BlockWidget::leaveEvent(QEvent *event)
{
    left_node->makeTransparent(true);
    right_node->makeTransparent(true);
}

void BlockWidget::paintEvent(QPaintEvent* event)
{

}

void BlockWidget::on_pushButton_clicked()
{
    auto text = resume_pause_button->text();
    if(text == "||")
        resume_pause_button->setText("▶");
    else
        resume_pause_button->setText("||");
}

void BlockWidget::on_leftCircle_clicked(ConnectNodeWidget* start_node)
{
    start_node->makeTransparent(false);
    qDebug() << "start from block";
    emit start(start_node);
}

void BlockWidget::on_rightCircle_clicked(ConnectNodeWidget* start_node)
{
    start_node->makeTransparent(false);
    qDebug() << "start from block";
    emit start(start_node);    
}

