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
    QAction *addRect = menu.addAction("Add RectItem");
    QAction *addLine = menu.addAction("Add LineItem");
    connect(addRect, &QAction::triggered, this, [=, this]{ static_cast<GraphicsScene*>(this->scene())->addRectItem(event->pos()); });
    connect(addLine, &QAction::triggered, this, [this]{ static_cast<GraphicsScene*>(this->scene())->addLine = true; });
    menu.exec(event->globalPos());
    return;
}
void GraphicsView::resizeEvent(QResizeEvent *event)
{
    this->scene()->setSceneRect(this->rect().x(), this->rect().y(), this->rect().width() - 10, this->rect().height() - 10);
    return QGraphicsView::resizeEvent(event);
}
GraphicsView::~GraphicsView(void){}