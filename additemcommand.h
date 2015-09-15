#ifndef ADDITEMCOMMAND_H
#define ADDRECTANGLECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>

class AddItemCommand : public QUndoCommand
{
    QGraphicsItem * m_item;
    QGraphicsScene * m_scene;
public:
    AddItemCommand(QGraphicsItem * item, QGraphicsScene * scene);

    void undo();
    void redo();
};

#endif // ADDRECTANGLECOMMAND_H
