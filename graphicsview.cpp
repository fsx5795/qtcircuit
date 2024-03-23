#include <QMenu>
#include <QContextMenuEvent>
#include "graphicsscene.h"
#include "graphicsview.h"
GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(new GraphicsScene());
}
void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    QAction *addAction = menu.addAction("Add RectItem");
    connect(addAction, &QAction::triggered, this, [=, this](void){ qobject_cast<GraphicsScene*>(this->scene())->addRectItem(QRect(event->pos(), QSize(100, 50))); });
    menu.exec(event->globalPos());
    return;
}
void GraphicsView::resizeEvent(QResizeEvent *event)
{
    this->scene()->setSceneRect(this->rect().x(), this->rect().y(), this->rect().width() - 10, this->rect().height() - 10);
    return QGraphicsView::resizeEvent(event);
}
GraphicsView::~GraphicsView(void){}