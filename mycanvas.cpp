#include "mycanvas.h"
#include <QDebug>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include "flute.h"
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>

QPen MyCanvas::itemPen(Qt::red);
QPen MyCanvas::steinerPen(Qt::black);
QPointF MyCanvas::gridSize(10, 100);
QColor MyCanvas::itemColor(Qt::red);

MyCanvas::MyCanvas(QWidget *parent) :
    QGraphicsView(parent),
    m_scene(new QGraphicsScene)
{
    setScene(m_scene.get());
    m_scene->setSceneRect(0,0,1000,1000);
    setDragMode(QGraphicsView::RubberBandDrag);
    setInteractive(true);
}

MyCanvas::~MyCanvas()
{

}

void MyCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << "double click " << mapToScene(event->pos());
    QPointF position = mapToScene(event->pos());
    QGraphicsScene* scene = m_scene.get();
    QGraphicsRectItem * item = scene->addRect(QRectF(QPointF(0,0), QSizeF(100.f, 100)), Qt::NoPen);
    item->setBrush(QBrush(MyCanvas::itemColor));
    item->setPos(position);
    item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

void MyCanvas::keyPressEvent(QKeyEvent *event)
{
    qDebug() << event;
    switch(event->key())
    {
    case Qt::Key_Delete:
        deleteSelectedItems();
        break;
    case Qt::Key_Space:
        connectSelectedItems();
        break;
    case Qt::Key_S:
        snapSelectedItems();
        break;
    }
}

void MyCanvas::drawBackground(QPainter *painter, const QRectF &rect)
{

    qreal left = int(rect.left()) - (int(rect.left()) % static_cast<int>(std::round(gridSize.x())));
    qreal top = int(rect.top()) - (int(rect.top()) % static_cast<int>(std::round(gridSize.y())));

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < rect.right(); x += static_cast<int>(std::round(gridSize.x())))
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += static_cast<int>(std::round(gridSize.y())))
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->setPen(QPen(Qt::gray));
    painter->drawLines(lines.data(), lines.size());

}

void MyCanvas::deleteSelectedItems()
{
    while(!scene()->selectedItems().empty())
    {
        QGraphicsItem * item = scene()->selectedItems().front();
        scene()->removeItem(item);
        delete item;
    }
}

void MyCanvas::connectSelectedItems()
{
    qDebug() << "Connect";

    int numItems = scene()->selectedItems().size();
    if(numItems <= 1)
        return;


    static bool read;
    if(!read)
    {
        readLUT();
        read = true;
    }

    std::vector<unsigned> x(numItems);
    std::vector<unsigned> y(numItems);

    x.resize(0);
    y.resize(0);

    for(auto item : scene()->selectedItems())
    {
        qDebug() << qgraphicsitem_cast<QGraphicsRectItem*>(item);

        unsigned currX = static_cast<unsigned>(std::round(item->x() + item->boundingRect().center().x()));
        unsigned currY = static_cast<unsigned>(std::round(item->y() + item->boundingRect().center().y()));
        x.push_back(currX);
        y.push_back(currY);
    }



    QGraphicsItemGroup * steiner = new QGraphicsItemGroup;
    if(numItems >= 3)
    {
        Tree flutetree = flute(numItems, x.data(), y.data(), ACCURACY);
        int branchnum = 2*flutetree.deg - 2;
        for(int j = 0; j < branchnum; ++j)
        {
            int n = flutetree.branch[j].n;
            if(j == n) continue;

            if(flutetree.branch[j].x == flutetree.branch[n].x || flutetree.branch[j].y == flutetree.branch[n].y)
            {
                QPointF pointA(static_cast<double>(flutetree.branch[j].x), static_cast<double>(flutetree.branch[j].y));
                QPointF pointB(static_cast<double>(flutetree.branch[n].x), static_cast<double>(flutetree.branch[n].y));
                QGraphicsLineItem* line = new QGraphicsLineItem(QLineF(pointA, pointB));
                line->setPen(MyCanvas::steinerPen);
                steiner->addToGroup(line);
            } else
            {
                QPointF pointA(static_cast<double>(flutetree.branch[j].x), static_cast<double>(flutetree.branch[j].y));
                QPointF pointB(static_cast<double>(flutetree.branch[j].x), static_cast<double>(flutetree.branch[n].y));
                QPointF pointC(static_cast<double>(flutetree.branch[n].x), static_cast<double>(flutetree.branch[n].y));
                QGraphicsLineItem* lineAB = new QGraphicsLineItem(QLineF(pointA, pointB));
                QGraphicsLineItem* lineBC = new QGraphicsLineItem(QLineF(pointB, pointC));
                lineAB->setPen(MyCanvas::steinerPen);
                lineBC->setPen(MyCanvas::steinerPen);
                steiner->addToGroup(lineAB);
                steiner->addToGroup(lineBC);
            }
        }
    } else {
        QPointF pointA(static_cast<double>(x[0]), static_cast<double>(y[0]));
        QPointF pointB(static_cast<double>(x[0]), static_cast<double>(y[1]));
        QPointF pointC(static_cast<double>(x[1]), static_cast<double>(y[1]));
        QGraphicsLineItem* lineAB = new QGraphicsLineItem(QLineF(pointA, pointB));
        QGraphicsLineItem* lineBC = new QGraphicsLineItem(QLineF(pointB, pointC));
        lineAB->setPen(MyCanvas::steinerPen);
        lineBC->setPen(MyCanvas::steinerPen);
        steiner->addToGroup(lineAB);
        steiner->addToGroup(lineBC);
    }
    steiner->setFlags(QGraphicsItem::ItemIsSelectable);
    scene()->addItem(steiner);
}

void MyCanvas::snapSelectedItems()
{
    for(auto item : scene()->selectedItems())
    {
        QPointF newPos(item->pos().x()/MyCanvas::gridSize.x(), item->pos().y()/MyCanvas::gridSize.y());
        newPos.setX(std::round(newPos.x()));
        newPos.setY(std::round(newPos.y()));
        newPos.setX(newPos.x()*MyCanvas::gridSize.x());
        newPos.setY(newPos.y()*MyCanvas::gridSize.y());
        item->setPos(newPos);
    }
}

void MyCanvas::critical(bool crit)
{
    MyCanvas::itemColor = (crit? QColor(Qt::red) : QColor(Qt::green));
}

