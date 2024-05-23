#include "connectnodewidget.h"

#include <QPainter>
#include <QDebug>

ConnectNodeWidget::ConnectNodeWidget(NodeType type, QWidget *parent)
    : QLabel(parent),
      _type(type)
{
    QPixmap pixmap(11, 11);
    setPixmap(pixmap);
    makeTransparent(true);
    setMouseTracking(true);
}

NodeType ConnectNodeWidget::getNodeType() const
{
    return _type;
}

void ConnectNodeWidget::makeTransparent(bool is_transparent)
{
    if(is_transparent)
    {
        QPixmap pm(*pixmap());
        pm.fill(Qt::transparent);
        setPixmap(pm);
    }
    else
    {
        QPixmap pm(*pixmap());
        QPainter painter(&pm);
        painter.setBrush(QBrush(Qt::green));
        auto radius = 5;
        auto point = QPoint(radius, radius);
        painter.drawEllipse(point, radius, radius);
        setPixmap(pm);
    }
}

void ConnectNodeWidget::mousePressEvent(QMouseEvent *event)
{
    emit start(this);
    qDebug() << "start from node";
}
