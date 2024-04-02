#ifndef COMPONENT_H
#define COMPONENT_H

#include <QGraphicsItemGroup>
#include <QGraphicsSceneContextMenuEvent>

enum class Direct
{
	Left,
	Top,
	Right,
	Bottom
};

class Component : public QGraphicsItemGroup
{
public:
	explicit Component(const QPoint&, QGraphicsItem *parent = nullptr);
	//virtual QRectF boundingRect() const override;
	std::array<std::tuple<QPointF, Direct>, 2> points() const noexcept;
	bool hasLine{false};

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent*) override;

private:
	QRectF boundRect;
	std::array<QGraphicsLineItem*, 2> lines;
};

#endif