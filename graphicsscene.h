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
    struct GraphicsModule
    {
        QGraphicsRectItem *startRect;
        Component *startCom;
        Direct startDrc;
        std::array<QGraphicsLineItem*, 3> lineList;
        QGraphicsRectItem *endRect;
        Component *endCom;
        Direct endDrc;
    };
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;
    void addRectItem(const QPoint&);
    bool drawLine{false};

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;

private:
    //新建立的矩形图元与上一个个矩形图元之间连线
    void new_rect_connect_line(QGraphicsRectItem*, QGraphicsRectItem*);
    //两个矩形图元之间连线
    std::vector<QLineF> rect_connect_line(QGraphicsRectItem*, QGraphicsRectItem*);
    //更新连线的线段
    void change_lines(std::list<QGraphicsLineItem*>*, const std::vector<QLineF>&);
    void change_lines(GraphicsModule&);
    //删除多余的线段
    void delete_lines(std::list<QGraphicsLineItem*>*, int);
    //添加缺少的线段
    void create_line(std::list<QGraphicsLineItem*>*, const QLineF&);
    std::list<GraphicsModule> moduleList;
    void leftRightHorizontal(Direct, const QPointF&) noexcept;
    std::list<std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>>> graphicsMoudules;
    bool drawEndLine{false};
};

#endif