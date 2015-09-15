#include "mycanvas.h"
#include <QDebug>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include "flute.h"
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <array>
#include <QtSvg>
#include "additemcommand.h"



QPen MyCanvas::itemPen(Qt::red);
QPen MyCanvas::steinerPen(Qt::black);
QPointF MyCanvas::gridSize(10, 100);
QColor MyCanvas::itemColor(Qt::red);
std::default_random_engine MyCanvas::engine;

MyCanvas::MyCanvas(QWidget *parent) :
    QGraphicsView(parent),
    m_scene(new QGraphicsScene)
{
    setScene(m_scene.get());
    m_scene->setSceneRect(0,0,20000,20000);
    setDragMode(QGraphicsView::RubberBandDrag);
    setInteractive(true);
    QPen cosmetic;
    cosmetic.setWidth(3);
    cosmetic.setCosmetic(true);
    scene()->addRect(0,0,5000,5000)->setPen(cosmetic);
}

MyCanvas::~MyCanvas()
{

}

void MyCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    static std::array<int, 5> sizes{12,24,36,48,96};
    std::uniform_int_distribution<int> distribution(0, 4);
    std::normal_distribution<double> normal(5.0, 3.0);
    qDebug() << "double click " << mapToScene(event->pos());

    double normalValue = normal(engine);
    int sizeIndex = 0;
    if(normalValue > 0 && normalValue <= 2.5)
        sizeIndex = 1;
    else if(normalValue > 2.5 && normalValue <= 5.0)
        sizeIndex = 2;
    else if(normalValue > 5.0 && normalValue <= 7.5)
        sizeIndex = 3;
    else if(normalValue > 7.5 && normalValue <= 10.0)
        sizeIndex = 4;
    QPointF position = mapToScene(event->pos());
    QGraphicsScene* scene = m_scene.get();
    QGraphicsRectItem* item = new QGraphicsRectItem(QRectF(QPointF(0,0), QSizeF(MyCanvas::gridSize.x()*sizes[sizeIndex], MyCanvas::gridSize.y())));
    item->setPen(Qt::NoPen);
    item->setBrush(QBrush(MyCanvas::itemColor));
    item->setPos(position);
    item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    m_stack.push(new AddItemCommand(item, scene));

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
    case Qt::Key_Z:
        if(QApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            if(m_stack.canRedo() && (QApplication::keyboardModifiers() & Qt::ShiftModifier))
                m_stack.redo();
            else if(m_stack.canUndo())
                m_stack.undo();
        }
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

void MyCanvas::wheelEvent(QWheelEvent *event)
{
    if(event->delta()>0)
        scale(1.1f, 1.1f);
    else
        scale(1.f/1.1f, 1.f/1.1f);
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
    m_stack.push(new AddItemCommand(steiner, scene()));
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

void MyCanvas::enablePan(bool arg)
{
    setInteractive(!arg);
    setDragMode(QGraphicsView::RubberBandDrag);
    if(arg)
        setDragMode(DragMode::ScrollHandDrag);
}

void MyCanvas::saveSVG(const QString &filename)
{
    QSvgGenerator svgGen;

    svgGen.setFileName( filename );
    svgGen.setSize(QSize(200, 200));
    svgGen.setViewBox(QRect(0, 0, 200, 200));
    svgGen.setTitle(tr("SVG Generator Example Drawing"));
    svgGen.setDescription(tr("An SVG drawing created by the SVG Generator "
                             "Example provided with Qt."));

    QPainter painter( &svgGen );
    scene()->render( &painter );
}

