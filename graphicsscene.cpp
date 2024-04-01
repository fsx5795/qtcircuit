#include <QGraphicsView>
#include "graphicsscene.h"

void GraphicsScene::addRectItem(const QPoint &pos)
{
    Component *com = new Component(pos);
    this->addItem(com);
    /*
    QList<QGraphicsItem*> items = this->items();
    for (int i{0}; i < items.count(); ++i) {
        QGraphicsItem *curItem = items.at(i);
        //当找到第一个矩形图元时（上一次添加的矩形图元）
        if (curItem->type() == QGraphicsRectItem::Type && curItem != rectItem) {
            //将上一次添加的矩形图元和刚刚添加的矩形图元连线后直接跳出
            this->new_rect_connect_line(static_cast<QGraphicsRectItem*>(curItem), rectItem);
            break;
        }
    }
    */
    return;
}
//新建立的矩形图元与上一个个矩形图元之间连线
void GraphicsScene::new_rect_connect_line(QGraphicsRectItem *startRectItem, QGraphicsRectItem *endRectItem)
{
    if (!startRectItem || !endRectItem)
        return;
    //用于保存两个图元及它们之间连线的结构体
    struct GraphicsModule module;
    module.startRect = startRectItem;
    module.endRect = endRectItem;
    //连线
    std::vector<QLineF> lineFvec = this->rect_connect_line(startRectItem, endRectItem);
    //将连线添加到结构体
    //this->change_lines(&module.lineList, lineFvec);
    this->moduleList.push_back(module);
    return;
}
//两个矩形图元之间连线
std::vector<QLineF> GraphicsScene::rect_connect_line(QGraphicsRectItem *startRectItem, QGraphicsRectItem *endRectItem)
{
    QRectF itemRect1 = startRectItem->sceneBoundingRect();
    QRectF itemRect2 = endRectItem->sceneBoundingRect();
    qreal startX = itemRect1.x() + itemRect1.width() + 1;
    qreal startY = itemRect1.topRight().y() + itemRect1.height() / 2;
    qreal endX = itemRect2.x() - 1;
    qreal endY = itemRect2.topRight().y() + itemRect2.height() / 2;
    std::vector<QLineF> lineFvec;
    if (startY == endY) {
        if (startX < endX)
           lineFvec.push_back(QLineF(startX, startY, endX, endY));
        else {
           lineFvec.push_back(QLineF(startX, startY, startX + 10, startY));
           lineFvec.push_back(QLineF(startX + 10, startY, endX - 10, endY));
           lineFvec.push_back(QLineF(endX - 10, endY, endX, endY));
        }
    }
    else if (startX + 20 < endX) {
        qreal midX = (endX - startX) / 2 + startX;
        lineFvec.push_back(QLineF(startX, startY, midX, startY));
        lineFvec.push_back(QLineF(midX, startY, midX, endY));
        lineFvec.push_back(QLineF(midX, endY, endX, endY));
    } 
    else {
        qreal midY = startY < endY ? (endY - startY) / 2 + startY : (startY - endY) / 2 + endY;
        lineFvec.push_back(QLineF(startX, startY, startX + 10, startY));
        lineFvec.push_back(QLineF(startX + 10, startY, startX + 10, midY));
        lineFvec.push_back(QLineF(startX + 10, midY, endX - 10, midY));
        lineFvec.push_back(QLineF(endX - 10, midY, endX - 10, endY));
        lineFvec.push_back(QLineF(endX - 10, endY, endX, endY));
    }
    return lineFvec;
}
//更新连线的线段
void GraphicsScene::change_lines(std::list<QGraphicsLineItem*> *lineList, const std::vector<QLineF> &lineFvec)
{
    //删除多余的线段
    this->delete_lines(lineList, lineFvec.size());
    int index{0};
    //更新原有的线段为新的线段
    for (auto it = lineList->cbegin(); it != lineList->cend(); ++it) {
        (*it)->setLine(lineFvec.at(index));
        ++index;
    }
    //如果原有线段数量少于新的线段数量
    while (index < lineFvec.size()) {
        //添加缺少的线段
        this->create_line(lineList, lineFvec.at(index));
        ++index;
    }
    return;
}
//更新连线
void GraphicsScene::change_lines(GraphicsModule &module)
{
    //创建新的连线线段
    std::vector<QLineF> lineFvec = this->rect_connect_line(module.startRect, module.endRect);
    //更新原有线段为新的连线线段
    //this->change_lines(&module.lineList, lineFvec);
    return;
}
//删除多余的线段
void GraphicsScene::delete_lines(std::list<QGraphicsLineItem*> *lineList, int count)
{
    //如果原有线段超过新的线段数量则删除多出来的线段
    while (lineList->size() > count) {
        QGraphicsLineItem *line = lineList->back();
        this->removeItem(line);
        lineList->pop_back();
        delete line;
        line = nullptr;
    }
    return;
}
//添加缺少的线段
void GraphicsScene::create_line(std::list<QGraphicsLineItem*> *lineList, const QLineF &lineF)
{
    QGraphicsLineItem *line = new QGraphicsLineItem(lineF);
    this->addItem(line);
    lineList->push_back(line);
    return;
}
void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (this->drawLine) {
        QPointF mousePointF = mouseEvent->scenePos();
        if (this->drawEndLine) {
            /*
            std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule = this->graphicsMoudules.back();
			for (QGraphicsItem *item : this->items()) {
                //如果当前遍历到的是元件
                if (item->type() == QGraphicsItemGroup::Type && std::get<0>(graphicsModule).at(0) != item) {
                    //元件的两个节点
                    std::array<std::tuple<QPointF, Direct>, 2> pd = static_cast<Component*>(item)->points();
                    if (sqrt((std::get<0>(pd.at(0)).x() - mousePointF.x()) + (std::get<0>(pd.at(0)).y() - mousePointF.y())) <= 10) {
                        switch (std::get<1>(graphicsModule).at(0)) {
                            case Direct::Left:
                            case Direct::Right:
                                this->leftRightHorizontal(std::get<1>(graphicsModule).at(0), std::get<0>(pd.at(0)));
                                break;
                            default:
                                break;
                        }
                        this->drawLine = false;
                        this->drawEndLine = false;
                        this->views().at(0)->setMouseTracking(false);
                    } else if(sqrt((std::get<0>(pd.at(1)).x() - mousePointF.x()) + (std::get<0>(pd.at(1)).y() - mousePointF.y())) <= 10) {
                        switch (std::get<1>(graphicsModule).at(0)) {
                            case Direct::Left:
                            case Direct::Right:
                                this->leftRightHorizontal(std::get<1>(graphicsModule).at(0), std::get<0>(pd.at(1)));
                                break;
                            default:
                                break;
                        }
                        this->drawLine = false;
                        this->drawEndLine = false;
                        this->views().at(0)->setMouseTracking(false);
                    }
                }
            }
            */
        } else {
			for (QGraphicsItem *item : this->items()) {
                if (item->type() == QGraphicsItemGroup::Type) {
                    std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> graphicsModule;
                    std::get<2>(graphicsModule).at(0) = nullptr;
                    std::get<2>(graphicsModule).at(1) = nullptr;
                    std::get<2>(graphicsModule).at(2) = nullptr;
                    //当前原件的两个节点
                    std::array<std::tuple<QPointF, Direct>, 2> pd = static_cast<Component*>(item)->points();
                    int i{-1};
                    //元件首节点
                    if (sqrt(pow(std::get<0>(pd.at(0)).x() - mouseEvent->scenePos().x(), 2) + pow(std::get<0>(pd.at(0)).y() - mouseEvent->scenePos().y(), 2)) <= 10)
                        i = 0;
                    //元件尾节点
                    else if(sqrt(pow(std::get<0>(pd.at(1)).x() - mouseEvent->scenePos().x(), 2) + pow(std::get<0>(pd.at(1)).y() - mouseEvent->scenePos().y(), 2)) <= 10)
                        i = 1;
                    if (i != -1) {
                        static_cast<Component*>(item)->hasLine = true;
                        //设置第一条线的起始坐标
                        std::get<2>(graphicsModule).at(0) = new QGraphicsLineItem(QLineF(std::get<0>(pd.at(i)), mouseEvent->scenePos()));
                        //标记第一条线所连元件节点的方向
                        std::get<1>(graphicsModule).at(0) = std::get<1>(pd.at(i));
                        //标记第一条线所连元件
                        std::get<0>(graphicsModule).at(0) = static_cast<Component*>(item);
                        this->graphicsMoudules.push_back(graphicsModule);
                        this->addItem(std::get<2>(graphicsModule).at(0));
                        this->drawEndLine = true;
                        this->views().at(0)->setMouseTracking(true);
                        break;
                    }
                }
            }
        }
    }
    return QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (this->drawLine && this->drawEndLine) {
        std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule = this->graphicsMoudules.back();
        QList<QGraphicsItem*> items = this->items();
        QList<QGraphicsItem*>::const_iterator item = items.constBegin();
        while (item != items.constEnd()) {
            //如果当前遍历到的是元件
            if ((*item)->type() == QGraphicsItemGroup::Type && std::get<0>(graphicsModule).at(0) != *item) {
                std::array<std::tuple<QPointF, Direct>, 2ULL> comps = static_cast<Component*>(*item)->points();
                QPointF mousePointF = mouseEvent->scenePos();
                int comIndex{0};
                //遍历当前元件的首尾节点
                while (comIndex < comps.size()) {
                    //如果所需链接的元件节点为元件当前节点
                    if (sqrt(pow(std::get<0>(comps.at(comIndex)).x() - mousePointF.x(), 2) + pow(std::get<0>(comps.at(comIndex)).y() - mousePointF.y(), 2)) <= 10)
                        break;
                    ++comIndex;
                }
                //如果当前有需要吸附的元件节点
                if (comIndex != comps.size()) {
                    //qDebug() << std::get<2>(graphicsModule).at(0)->line().x1() << std::get<0>(comps.at(comIndex)).x();
                    //水平或垂直
                    if (std::get<2>(graphicsModule).at(0)->y() == std::get<0>(comps.at(comIndex)).y() || std::get<2>(graphicsModule).at(0)->x() == std::get<0>(comps.at(comIndex)).x()) {
                        std::get<2>(graphicsModule).at(0)->setLine(QLineF(std::get<2>(graphicsModule).at(0)->line().p1(), std::get<0>(comps.at(comIndex))));
                        if (std::get<2>(graphicsModule).at(1) != nullptr) {
                            this->removeItem(std::get<2>(graphicsModule).at(1));
                            delete std::get<2>(graphicsModule).at(1);
                            std::get<2>(graphicsModule).at(1) = nullptr;
                        }
                        if (std::get<2>(graphicsModule).at(2) != nullptr) {
                            this->removeItem(std::get<2>(graphicsModule).at(2));
                            delete std::get<2>(graphicsModule).at(2);
                            std::get<2>(graphicsModule).at(2) = nullptr;
                        }
                    //朝左
                    } else if (std::get<2>(graphicsModule).at(0)->line().x1() > std::get<0>(comps.at(comIndex)).x()) {
                        switch (std::get<1>(graphicsModule).at(0)) {
                            //起始元件节点方向朝左
                            case Direct::Left:
                                switch (std::get<1>(comps.at(comIndex))) {
                                    //所需连接的元件节点方向
                                    case Direct::Left:
                                        this->targetBuck(true, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Top:
                                        //左下方朝x方向折，左上方朝y方向折
                                        this->targetBuck(std::get<2>(graphicsModule).at(0)->y() < std::get<0>(comps.at(comIndex)).y(), std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Right:
                                        this->targetMidX(std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Bottom:
                                        //左上方朝x方向折，左下方朝y方向折
                                        this->targetBuck(std::get<2>(graphicsModule).at(0)->y() > std::get<0>(comps.at(comIndex)).y(), std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                }
                                break;
                            case Direct::Top:
                                switch (std::get<1>(comps.at(comIndex))) {
                                    //所需连接的元件节点方向
                                    case Direct::Left:
                                        break;
                                    case Direct::Top:
                                        break;
                                    case Direct::Right:
                                        break;
                                    case Direct::Bottom:
                                        break;
                                }
                                break;
                            //起始元件节点方向朝右
                            case Direct::Right:
                                switch (std::get<1>(comps.at(comIndex))) {
                                    //所需连接的元件节点方向
                                    case Direct::Left:
                                        //左下方朝x方向折，左上方朝y方向折
                                        this->targetBuck(std::get<2>(graphicsModule).at(0)->y() < std::get<0>(comps.at(comIndex)).y(), std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Top:
                                        this->targetBuck(false, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Right:
                                        this->targetBuck(false, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Bottom:
                                        this->targetBuck(false, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                }
                                break;
                            case Direct::Bottom:
                                switch (std::get<1>(comps.at(comIndex))) {
                                    //所需连接的元件节点方向
                                    case Direct::Left:
                                        break;
                                    case Direct::Top:
                                        break;
                                    case Direct::Right:
                                        break;
                                    case Direct::Bottom:
                                        break;
                                }
                                break;
                        }
                    //朝右
                    } else {
                        switch (std::get<1>(graphicsModule).at(0)) {
                            //起始元件节点方向朝左
                            case Direct::Left:
                                switch (std::get<1>(comps.at(comIndex))) {
                                    //所需连接的元件节点方向
                                    case Direct::Left:
                                        this->targetBuck(false, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Top:
                                        this->targetBuck(false, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Right:
                                        this->targetMidY(std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Bottom:
                                        this->targetBuck(false, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                }
                                break;
                            case Direct::Top:
                                switch (std::get<1>(comps.at(comIndex))) {
                                    //所需连接的元件节点方向
                                    case Direct::Left:
                                        break;
                                    case Direct::Top:
                                        break;
                                    case Direct::Right:
                                        break;
                                    case Direct::Bottom:
                                        break;
                                }
                                break;
                            //起始元件节点方向朝右
                            case Direct::Right:
                                switch (std::get<1>(comps.at(comIndex))) {
                                    //所需连接的元件节点方向
                                    case Direct::Left:
                                        this->targetMidX(std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Top:
                                        this->targetBuck(false, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Right:
                                        this->targetBuck(true, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                    case Direct::Bottom:
                                        this->targetBuck(false, std::get<2>(graphicsModule), std::get<0>(comps.at(comIndex)));
                                        break;
                                }
                                break;
                            case Direct::Bottom:
                                switch (std::get<1>(comps.at(comIndex))) {
                                    //所需连接的元件节点方向
                                    case Direct::Left:
                                        break;
                                    case Direct::Top:
                                        break;
                                    case Direct::Right:
                                        break;
                                    case Direct::Bottom:
                                        break;
                                }
                                break;
                        }
                    }
                    //有要吸附的元件节点则直接跳出循环
                    break;
                }
            }
            ++item;
        }
        if (item == items.constEnd())
            this->leftRightHorizontal(std::get<1>(graphicsModule).at(0), mouseEvent->scenePos());
    } else {
        QList<QGraphicsItem*> selectItems = this->selectedItems();
        if (selectItems.size() > 0) {
            if (selectItems.at(0)->type() == QGraphicsItemGroup::Type) {
                Component *com = static_cast<Component*>(selectItems.at(0));
                if (com->hasLine) {
                    for (std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsMoudule : this->graphicsMoudules) {
                        //如果当前选择元件是作为起始元件
                        if (std::get<0>(graphicsMoudule).at(0) == com) {
                            std::array<std::tuple<QPointF, Direct>, 2> pd = com->points();
                            //如果当前连线的起始节点是当前元件的第一个节点(方向一致)
                            if (std::get<1>(graphicsMoudule).at(0) == std::get<1>(pd.at(0))) {
                                //获取尾线
                                int i{0};
                                while (i < std::get<2>(graphicsMoudule).size()) {
                                    if (std::get<2>(graphicsMoudule).at(i) == nullptr)
                                        break;
                                    ++i;
                                }
                                //鼠标点与起始点的位置关系(同边、反边、同线)
                                Bear bear;
                                switch (std::get<1>(pd.at(0))) {
                                    case Direct::Left:
                                        bear = std::get<0>(pd.at(0)).x() > std::get<2>(graphicsMoudule).at(i - 1)->line().x2() ? Bear::Positive : std::get<0>(pd.at(0)).x() < std::get<2>(graphicsMoudule).at(i - 1)->line().x2() ? Bear::Reverse : Bear::Same;
                                        break;
                                    case Direct::Right:
                                        bear = std::get<0>(pd.at(0)).x() < std::get<2>(graphicsMoudule).at(i - 1)->line().x2() ? Bear::Positive : std::get<0>(pd.at(0)).x() > std::get<2>(graphicsMoudule).at(i - 1)->line().x2() ? Bear::Reverse : Bear::Same;
                                        break;
                                    default:
                                        break;
                                    /*
                                    switch (bear) {
                                        case Bear::Positive:
                                            double midX;
                                            //当前选择元件的第一个节点方向
                                            switch (std::get<1>(pd.at(0))) {
                                                case Direct::Left:
                                                    midX = mousePointF.x() + abs(frontLineF.x1() - mousePointF.x()) / 2;
                                                    break;
                                                case Direct::Right:
                                                    midX = mousePointF.x() - abs(frontLineF.x1() - mousePointF.x()) / 2;
                                                    break;
                                                default:
                                                    break;
                                            }
                                            if (frontLineF.y1() != mousePointF.y()) {
                                                std::get<2>(graphicsModule).at(0)->setLine(frontLineF.x1(), frontLineF.y1(), midX, frontLineF.y1());
                                                if (std::get<2>(graphicsModule).at(1) == nullptr) {
                                                    std::get<2>(graphicsModule).at(1) = new QGraphicsLineItem(midX, frontLineF.y1(), midX, mousePointF.y());
                                                    this->addItem(std::get<2>(graphicsModule).at(1));
                                                } else {
                                                    std::get<2>(graphicsModule).at(1)->setLine(midX, frontLineF.y1(), midX, mousePointF.y());
                                                }
                                                if (std::get<2>(graphicsModule).at(2) == nullptr) {
                                                    std::get<2>(graphicsModule).at(2) = new QGraphicsLineItem(midX, mousePointF.y(), mousePointF.x(), mousePointF.y());
                                                    this->addItem(std::get<2>(graphicsModule).at(2));
                                                } else {
                                                    std::get<2>(graphicsModule).at(2)->setLine(midX, mousePointF.y(), mousePointF.x(), mousePointF.y());
                                                }
                                            } else {
                                                std::get<2>(graphicsModule).at(0)->setLine(frontLineF.x1(), frontLineF.y1(), mousePointF.x(), frontLineF.y1());
                                                for (int i{1}; i <= 2; ++i) {
                                                    if (std::get<2>(graphicsModule).at(i) != nullptr) {
                                                        this->removeItem(std::get<2>(graphicsModule).at(i));
                                                        delete std::get<2>(graphicsModule).at(i);
                                                        std::get<2>(graphicsModule).at(i) = nullptr;
                                                    }
                                                }
                                            }
                                            break;
                                        //如果鼠标点与起始点反边
                                        case Bear::Reverse:
                                            if (frontLineF.y1() != mousePointF.y()) {
                                                std::get<2>(graphicsModule).at(0)->setLine(frontLineF.x1(), frontLineF.y1(), frontLineF.x1(), mousePointF.y());
                                                if (std::get<2>(graphicsModule).at(1) == nullptr) {
                                                    std::get<2>(graphicsModule).at(1) = new QGraphicsLineItem(frontLineF.x1(), mousePointF.y(), mousePointF.x(), mousePointF.y());
                                                    this->addItem(std::get<2>(graphicsModule).at(1));
                                                } else {
                                                    std::get<2>(graphicsModule).at(1)->setLine(frontLineF.x1(), mousePointF.y(), mousePointF.x(), mousePointF.y());
                                                }
                                                if (std::get<2>(graphicsModule).at(2) != nullptr) {
                                                    this->removeItem(std::get<2>(graphicsModule).at(2));
                                                    delete std::get<2>(graphicsModule).at(2);
                                                    std::get<2>(graphicsModule).at(2) = nullptr;
                                                }
                                            }
                                            break;
                                        case Bear::Same:
                                            std::get<2>(graphicsModule).at(0)->setLine(frontLineF.x1(), frontLineF.y1(), mousePointF.x(), mousePointF.y());
                                            for (int i{1}; i <= 2; ++i) {
                                                if (std::get<2>(graphicsModule).at(i) != nullptr) {
                                                    this->removeItem(std::get<2>(graphicsModule).at(i));
                                                    delete std::get<2>(graphicsModule).at(i);
                                                    std::get<2>(graphicsModule).at(i) = nullptr;
                                                }
                                            }
                                            break;
                                    }
                                    */
                                    //std::get<2>(graphicsMoudule).at(0)->setLine(QLineF(std::get<0>(pd.at(0)),));
                                }
                                //更新第一条线
                            }
                        }
                    }
                }
            }
        }
    }
    return QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void GraphicsScene::targetBuck(bool isX, std::array<QGraphicsLineItem*, 3> &gmLines, const QPointF &comPf) noexcept
{
    //转折点
    QPointF pf = isX ? QPointF(comPf.x(), gmLines.at(0)->line().y1()) : QPointF(gmLines.at(0)->line().x1(), comPf.y());
    gmLines.at(0)->setLine(QLineF(gmLines.at(0)->line().p1(), pf));
    if (gmLines.at(1) != nullptr) {
        gmLines.at(1)->setLine(QLineF(pf, comPf));
    } else {
        gmLines.at(1) = new QGraphicsLineItem(QLineF(pf, comPf));
        this->addItem(gmLines.at(1));
    }
    if (gmLines.at(2) != nullptr) {
        this->removeItem(gmLines.at(2));
        delete gmLines.at(2);
        gmLines.at(2) = nullptr;
    }
    return;
}

void GraphicsScene::targetMidX(std::array<QGraphicsLineItem*, 3> &gmLines, const QPointF &comPf) noexcept
{
    double midX = comPf.x() < gmLines.at(0)->line().x1() ? comPf.x() + abs(gmLines.at(0)->line().x1() - comPf.x()) / 2 : comPf.x() - abs(gmLines.at(0)->line().x1() - comPf.x()) / 2;
    gmLines.at(0)->setLine(QLineF(gmLines.at(0)->line().p1(), QPointF(midX, gmLines.at(0)->line().y1())));
    if (gmLines.at(1) != nullptr) {
        gmLines.at(1)->setLine(midX, gmLines.at(0)->line().y1(), midX, comPf.y());
    } else {
        gmLines.at(1) = new QGraphicsLineItem(midX, gmLines.at(0)->line().y1(), midX, comPf.y());
        this->addItem(gmLines.at(1));
    }
    if (gmLines.at(2) != nullptr) {
        gmLines.at(2)->setLine(QLineF(QPointF(midX, comPf.y()), comPf));
    } else {
        gmLines.at(2) = new QGraphicsLineItem(QLineF(QPointF(midX, comPf.y()), comPf));
        this->addItem(gmLines.at(2));
    }
    return;
}

void GraphicsScene::targetMidY(std::array<QGraphicsLineItem*, 3> &gmLines, const QPointF &comPf) noexcept
{
    double midY = comPf.y() < gmLines.at(0)->line().y1() ? comPf.y() + abs(gmLines.at(0)->line().y1() - comPf.y()) / 2 : comPf.y() - abs(gmLines.at(0)->line().y1() - comPf.y()) / 2;
    gmLines.at(0)->setLine(QLineF(gmLines.at(0)->line().p1(), QPointF(gmLines.at(0)->line().x1(), midY)));
    if (gmLines.at(1) != nullptr) {
        gmLines.at(1)->setLine(gmLines.at(0)->line().x1(), midY, comPf.x(), midY);
    } else {
        gmLines.at(1) = new QGraphicsLineItem(gmLines.at(0)->line().x1(), midY, comPf.y(), midY);
        this->addItem(gmLines.at(1));
    }
    if (gmLines.at(2) != nullptr) {
        gmLines.at(2)->setLine(QLineF(QPointF(comPf.x(), midY), comPf));
    } else {
        gmLines.at(2) = new QGraphicsLineItem(QLineF(QPointF(comPf.x(), midY), comPf));
        this->addItem(gmLines.at(2));
    }
    return;
}


void GraphicsScene::leftRightHorizontal(Direct direct, const QPointF &mousePointF) noexcept
{
    std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule = this->graphicsMoudules.back();
    QLineF frontLineF = std::get<2>(graphicsModule).at(0)->line();
    //鼠标点与起始点的位置关系(同边、反边、同线)
    Bear bear;
    switch (direct) {
        case Direct::Left:
            bear = frontLineF.x1() > mousePointF.x() ? Bear::Positive : frontLineF.x1() < mousePointF.x() ? Bear::Reverse : Bear::Same;
            break;
        case Direct::Right:
            bear = frontLineF.x1() < mousePointF.x() ? Bear::Positive : frontLineF.x1() > mousePointF.x() ? Bear::Reverse : Bear::Same;
            break;
        default:
            break;
    }
    switch (bear) {
        //如果鼠标点与起始点同边
        case Bear::Positive:
            double midX;
            switch (direct) {
                case Direct::Left:
                    midX = mousePointF.x() + abs(frontLineF.x1() - mousePointF.x()) / 2;
                    break;
                case Direct::Right:
                    midX = mousePointF.x() - abs(frontLineF.x1() - mousePointF.x()) / 2;
                    break;
                default:
                    break;
            }
            if (frontLineF.y1() != mousePointF.y()) {
                std::get<2>(graphicsModule).at(0)->setLine(frontLineF.x1(), frontLineF.y1(), midX, frontLineF.y1());
                if (std::get<2>(graphicsModule).at(1) == nullptr) {
                    std::get<2>(graphicsModule).at(1) = new QGraphicsLineItem(midX, frontLineF.y1(), midX, mousePointF.y());
                    this->addItem(std::get<2>(graphicsModule).at(1));
                } else {
                    std::get<2>(graphicsModule).at(1)->setLine(midX, frontLineF.y1(), midX, mousePointF.y());
                }
                if (std::get<2>(graphicsModule).at(2) == nullptr) {
                    std::get<2>(graphicsModule).at(2) = new QGraphicsLineItem(midX, mousePointF.y(), mousePointF.x(), mousePointF.y());
                    this->addItem(std::get<2>(graphicsModule).at(2));
                } else {
                    std::get<2>(graphicsModule).at(2)->setLine(midX, mousePointF.y(), mousePointF.x(), mousePointF.y());
                }
            } else {
                std::get<2>(graphicsModule).at(0)->setLine(frontLineF.x1(), frontLineF.y1(), mousePointF.x(), frontLineF.y1());
                for (int i{1}; i <= 2; ++i) {
                    if (std::get<2>(graphicsModule).at(i) != nullptr) {
                        this->removeItem(std::get<2>(graphicsModule).at(i));
                        delete std::get<2>(graphicsModule).at(i);
                        std::get<2>(graphicsModule).at(i) = nullptr;
                    }
                }
            }
            break;
        //如果鼠标点与起始点反边
        case Bear::Reverse:
            if (frontLineF.y1() != mousePointF.y()) {
                std::get<2>(graphicsModule).at(0)->setLine(frontLineF.x1(), frontLineF.y1(), frontLineF.x1(), mousePointF.y());
                if (std::get<2>(graphicsModule).at(1) == nullptr) {
                    std::get<2>(graphicsModule).at(1) = new QGraphicsLineItem(frontLineF.x1(), mousePointF.y(), mousePointF.x(), mousePointF.y());
                    this->addItem(std::get<2>(graphicsModule).at(1));
                } else {
                    std::get<2>(graphicsModule).at(1)->setLine(frontLineF.x1(), mousePointF.y(), mousePointF.x(), mousePointF.y());
                }
                if (std::get<2>(graphicsModule).at(2) != nullptr) {
                    this->removeItem(std::get<2>(graphicsModule).at(2));
                    delete std::get<2>(graphicsModule).at(2);
                    std::get<2>(graphicsModule).at(2) = nullptr;
                }
            }
            break;
        case Bear::Same:
            std::get<2>(graphicsModule).at(0)->setLine(frontLineF.x1(), frontLineF.y1(), mousePointF.x(), mousePointF.y());
            for (int i{1}; i <= 2; ++i) {
                if (std::get<2>(graphicsModule).at(i) != nullptr) {
                    this->removeItem(std::get<2>(graphicsModule).at(i));
                    delete std::get<2>(graphicsModule).at(i);
                    std::get<2>(graphicsModule).at(i) = nullptr;
                }
            }
            break;
        }
        return;
    }