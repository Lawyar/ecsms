#include "blockwidget.h"
#include "connectnodewidget.h"
#include "blockfield.h"

#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

BlockWidget::BlockWidget(BlockField *parent)
    : QWidget(parent),
      _block_name(new QLabel("block name", this)),
      _left_node(new ConnectNodeWidget(Incoming, this)),
      _right_node(new ConnectNodeWidget(Outgoing, this)),
      _resume_pause_button(new QPushButton("||", this))
{
    connect(_left_node, &ConnectNodeWidget::start, this, &BlockWidget::on_leftCircle_clicked);
    connect(_right_node, &ConnectNodeWidget::start, this, &BlockWidget::on_rightCircle_clicked);

    connect(this, &BlockWidget::start, parent, &BlockField::on_start);

    /*connect(left_node, &ConnectNodeWidget::end, this, &BlockWidget::on_leftCircle_clicked);
    connect(right_node, &ConnectNodeWidget::end, this, &BlockWidget::on_rightCircle_clicked);*/

    _block_name->setWordWrap(true);
    _block_name->setAlignment(Qt::AlignCenter);
    _block_name->setFrameStyle(QFrame::Box | QFrame::Plain);
    auto new_font = _block_name->font();
    new_font.setPointSize(12);
    _block_name->setFont(new_font);
    _resume_pause_button->setFont(new_font);

    _block_name->show();
    _resume_pause_button->show();

    auto label_width = _block_name->width();
    auto label_height = _block_name->height();
    auto spacing = 2, spacing2 = 3;
    auto button_width = _resume_pause_button->width();
    auto button_height = _resume_pause_button->height();
    auto radius = 5, diameter = radius * 2;

    auto width = 2 * (2 * spacing + diameter) + label_width;
    auto height = 2 * spacing + button_height + spacing2 + label_height;
    setGeometry(0,0, width, height);

    _resume_pause_button->move((width - button_width) / 2, spacing);
    _block_name->move(2 * spacing + diameter, spacing + button_height + spacing2);

    _left_node->move(spacing, spacing + button_height + spacing2 + label_height / 2 - diameter / 2);
    _right_node->move(width - spacing - diameter, spacing + button_height + spacing2 + label_height / 2 - diameter / 2);

    setMouseTracking(true);

    connect(_resume_pause_button, &QPushButton::clicked, this, &BlockWidget::on_pushButton_clicked);
}

ConnectNodeWidget *BlockWidget::GetLeftNode() { return _left_node; }

ConnectNodeWidget *BlockWidget::GetRightNode() { return _right_node; }

void BlockWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        _old_pos = event->pos();
        parentWidget()->repaint();
    }
}

void BlockWidget::mouseMoveEvent(QMouseEvent *event)
{
    _left_node->makeTransparent(false);
    _right_node->makeTransparent(false);
    if (event->buttons() ==  Qt::LeftButton)
    {
        QPoint delta = event->pos() - _old_pos;
        move(pos() + delta);
        parentWidget()->repaint();
    }
}

void BlockWidget::leaveEvent(QEvent *event)
{
    _left_node->makeTransparent(true);
    _right_node->makeTransparent(true);
}

void BlockWidget::paintEvent(QPaintEvent* event)
{

}

void BlockWidget::on_pushButton_clicked()
{
    auto text = _resume_pause_button->text();
    if(text == "||")
        _resume_pause_button->setText("▶");
    else
        _resume_pause_button->setText("||");
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

