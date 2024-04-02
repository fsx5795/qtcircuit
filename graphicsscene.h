#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "component.h"

enum class Bear
{
    Positive,
    Reverse,
    Same
};

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;
    void addRectItem(const QPoint&);
    bool addLine{false};

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;

private:
    bool drawEndLine(bool, const QPointF&) noexcept;
    void targetBuck(bool, std::array<QGraphicsLineItem*, 3>&, const std::array<QPointF, 2>&) noexcept;
    void targetMidX(std::array<QGraphicsLineItem*, 3>&, const std::array<QPointF, 2>&) noexcept;
    void targetMidY(std::array<QGraphicsLineItem*, 3>&, const std::array<QPointF, 2>&) noexcept;
    void leftRightHorizontal(Direct, const QPointF&) noexcept;
    void updateLines(std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>>&, const std::array<std::tuple<QPointF, Direct>, 2>&) noexcept;
    std::optional<std::tuple<QPointF, Direct>> getComponentNode(const std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>>&, int) noexcept;
    std::list<std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>>> graphicsModules;
    bool addEndLine{false};
};

#endif