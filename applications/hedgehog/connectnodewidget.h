#ifndef CONNECTNODEWIDGET_H
#define CONNECTNODEWIDGET_H

#include "blockwidget.h"

#include <QLabel>

enum NodeType
{
    Incoming, Outgoing
};

class ConnectNodeWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ConnectNodeWidget(NodeType type, QWidget *parent = nullptr);

    NodeType getNodeType() const;

    void makeTransparent(bool value);

	QPoint coordToParent() const;

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void start(ConnectNodeWidget* start_node);

    void end(ConnectNodeWidget* end_node);

private:
    NodeType _type;
};

#endif // CONNECTNODEWIDGET_H
