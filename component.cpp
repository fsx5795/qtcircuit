#include <QMenu>
#include <QGraphicsRectItem>
#include "component.h"

Component::Component(const QPoint &pos, QGraphicsItem *parent)
{
	QGraphicsRectItem *rectItem = new QGraphicsRectItem(pos.x(), pos.y(), 100, 50);
	QRectF rect = rectItem->rect();
	double midHeight = rect.y() + rect.height() / 2;
	this->lines.at(0) = new QGraphicsLineItem(rect.x() - 15, midHeight, rect.x(), midHeight);
	this->lines.at(1) = new QGraphicsLineItem(rect.x() + rect.width(), midHeight, rect.x() + rect.width() + 15, midHeight);
    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);

	this->addToGroup(rectItem);
	this->addToGroup(this->lines.at(0));
	this->addToGroup(this->lines.at(1));
	/*
	this->boundRect.setX(leftLine->line().x1());
	this->boundRect.setY(rect.y());
	this->boundRect.setWidth(rightLine->line().y2() - leftLine->line().x1());
	this->boundRect.setHeight(rectItem->rect().height() + 10);
	*/
}
/*
QRectF Component::boundingRect() const
{
	return this->boundRect;
}
*/
void Component::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu menu;
	QAction *rotatAction = menu.addAction("Rotation");
	menu.exec(event->screenPos());
	return QGraphicsItem::contextMenuEvent(event);
}
std::array<std::tuple<QPointF, Direct>, 2> Component::points() const noexcept
{
	return std::array<std::tuple<QPointF, Direct>, 2>{std::make_tuple(this->mapToScene(this->lines.at(0)->line().p1()), Direct::Left), std::make_tuple(this->mapToScene(this->lines.at(1)->line().p2()), Direct::Right)};
}
