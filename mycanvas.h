#ifndef MYCANVAS_H
#define MYCANVAS_H

#include <QGraphicsView>
#include <memory>

class MyCanvas : public QGraphicsView
{
    std::unique_ptr<QGraphicsScene> m_scene;

    static QPen itemPen;
    static QPen steinerPen;
    static QPointF gridSize;
    static QColor itemColor;
    static std::default_random_engine engine;


public:
    explicit MyCanvas(QWidget* parent = 0);
    ~MyCanvas();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void wheelEvent(QWheelEvent*event);

    void deleteSelectedItems();
    void connectSelectedItems();
    void snapSelectedItems();

    void critical(bool crit);
};

#endif // MYCANVAS_H
