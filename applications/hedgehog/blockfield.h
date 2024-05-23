#ifndef BLOCKFIELD_H
#define BLOCKFIELD_H

#include <QWidget>
#include <QMap>

class ConnectNodeWidget;

class BlockField : public QWidget
{
    Q_OBJECT
public:
    BlockField(QWidget* parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event)  override;

    void mousePressEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    static QPoint coordToBlockField(ConnectNodeWidget* n);

public slots:
    void on_start(ConnectNodeWidget* start);

private:
    QPoint pos;
    ConnectNodeWidget* selected_node = nullptr;
    QMap<ConnectNodeWidget*, QVector<ConnectNodeWidget*>> _map_of_selected_nodes;
    QMap<ConnectNodeWidget*, QVector<ConnectNodeWidget*>> _connection_map;
};

#endif // BLOCKFIELD_H
