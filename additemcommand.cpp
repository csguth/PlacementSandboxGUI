#include "additemcommand.h"

AddItemCommand::AddItemCommand(QGraphicsItem *item, QGraphicsScene *scene):
    m_item(item),
    m_scene(scene)
{
}

void AddItemCommand::undo()
{
    if(std::find(m_scene->items().begin(), m_scene->items().end(), m_item) != m_scene->items().end())
        m_scene->removeItem(m_item);
    else
    {
        delete m_item;
        m_item = nullptr;
    }
}

void AddItemCommand::redo()
{
    if(m_item)
        m_scene->addItem(m_item);
}
