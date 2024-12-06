#include <cmath>
#include <QGraphicsView>
#include "graphicsscene.h"

void GraphicsScene::addRectItem(const QPoint &pos)
{
    Component *com = new Component(pos);
    this->addItem(com);
    return;
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (this->addLine) {
        if (this->addEndLine) {
            if (this->drawEndLine(true, mouseEvent->scenePos())) {
                this->addLine = false;
                this->addEndLine = false;
                this->views().at(0)->setMouseTracking(false);
            }
        } else {
			for (QGraphicsItem *item : this->items()) {
                if (item->type() == QGraphicsItemGroup::Type) {
                    Component *com = static_cast<Component*>(item);
                    std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> graphicsModule;
                    std::get<2>(graphicsModule).at(0) = nullptr;
                    std::get<2>(graphicsModule).at(1) = nullptr;
                    std::get<2>(graphicsModule).at(2) = nullptr;
                    //当前原件的两个节点
                    std::array<std::tuple<QPointF, Direct>, 2> pd = com->points();
                    int i{0};
                    while (i < pd.size()) {
                        if (sqrt(pow(std::get<0>(pd.at(i)).x() - mouseEvent->scenePos().x(), 2) + pow(std::get<0>(pd.at(i)).y() - mouseEvent->scenePos().y(), 2)) <= 10)
                            break;
                        ++i;
                    }
                    if (i != pd.size()) {
                        com->hasLine = true;
                        //设置第一条线的起始坐标
                        std::get<2>(graphicsModule).at(0) = new QGraphicsLineItem(QLineF(std::get<0>(pd.at(i)), mouseEvent->scenePos()));
                        //标记第一条线所连元件节点的方向
                        std::get<1>(graphicsModule).at(0) = std::get<1>(pd.at(i));
                        //标记第一条线所连元件
                        std::get<0>(graphicsModule).at(0) = com;
                        this->graphicsModules.push_back(graphicsModule);
                        this->addItem(std::get<2>(graphicsModule).at(0));
                        this->addEndLine = true;
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
    QPointF mousePointF = mouseEvent->scenePos();
    if (this->addLine && this->addEndLine) {
        this->drawEndLine(false, mousePointF);
    }
    //} else {
        QList<QGraphicsItem*> selectItems = this->selectedItems();
        if (selectItems.size() > 0) {
            if (selectItems.at(0)->type() == QGraphicsItemGroup::Type) {
                Component *com = static_cast<Component*>(selectItems.at(0));
                if (com->hasLine) {
                    std::array<std::tuple<QPointF, Direct>, 2> pd = com->points();
                    for (std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule : this->graphicsModules) {
                        //如果当前选择元件是作为起始元件
                        if (std::get<0>(graphicsModule).at(0) == com) {
                            //找到尾元件节点
                            std::optional<std::tuple<QPointF, Direct>> comNode = this->getComponentNode(graphicsModule, 1);
                            if (comNode.has_value()) {
                                //如果当前连线的起始节点是当前元件的第一个节点(方向一致)
                                if (std::get<1>(graphicsModule).at(0) == std::get<1>(pd.at(0))) {
                                    this->updateLines(graphicsModule, {pd.at(0), *comNode});
                                } else {
                                    this->updateLines(graphicsModule, {pd.at(1), *comNode});
                                }
                            }
                        //如果当前选择的元件是作为收尾元件
                        } else if (std::get<0>(graphicsModule).at(1) == com) {
                            //找到首元件节点
                            std::optional<std::tuple<QPointF, Direct>> comNode = this->getComponentNode(graphicsModule, 0);
                            if (comNode.has_value()) {
                                //如果当前连线的起始节点是当前元件的第一个节点(方向一致)
                                if (std::get<1>(graphicsModule).at(1) == std::get<1>(pd.at(0))) {
                                    this->updateLines(graphicsModule, {*comNode, pd.at(0)});
                                } else {
                                    this->updateLines(graphicsModule, {*comNode, pd.at(1)});
                                }
                            }
                        }
                    }
                }
            }
        }
    //}
    return QGraphicsScene::mouseMoveEvent(mouseEvent);
}

std::optional<std::tuple<QPointF, Direct>> GraphicsScene::getComponentNode(const std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule, int index) noexcept
{
    std::optional<std::tuple<QPointF, Direct>> result = std::nullopt;
    if (std::get<0>(graphicsModule).at(index) != nullptr) {
        std::array<std::tuple<QPointF, Direct>, 2> comps = std::get<0>(graphicsModule).at(index)->points();
        int comIndex{0};
        while (comIndex != comps.size()) {
            //方向一致
            if (std::get<1>(graphicsModule).at(index) == std::get<1>(comps.at(comIndex)))
                break;
            ++comIndex;
        }
        if (comIndex != comps.size())
            result = comps.at(comIndex);
    }
    return result;
}

bool GraphicsScene::drawEndLine(bool linkEnd, const QPointF &mousePointF) noexcept
{
    std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule = this->graphicsModules.back();
    QList<QGraphicsItem*> items = this->items();
    QList<QGraphicsItem*>::const_iterator item = items.constBegin();
    while (item != items.constEnd()) {
        //如果当前遍历到的是元件
        if ((*item)->type() == QGraphicsItemGroup::Type && std::get<0>(graphicsModule).at(0) != *item) {
            std::array<std::tuple<QPointF, Direct>, 2ULL> comps = static_cast<Component*>(*item)->points();
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
                //找到首元件节点
                std::optional<std::tuple<QPointF, Direct>> comNode = this->getComponentNode(graphicsModule, 0);
                if (comNode.has_value())
                    this->updateLines(graphicsModule, {*comNode, comps.at(comIndex)});
                if (linkEnd) {
                    Component *com = static_cast<Component*>(*item);
                    std::get<0>(graphicsModule).at(1) = com;
                    com->hasLine = true;
                    //标记所连元件节点的方向
                    std::get<1>(graphicsModule).at(1) = std::get<1>(comps.at(comIndex));
                }
                //有要吸附的元件节点则直接跳出循环
                break;
            }
        }
        ++item;
    }
    bool flag = item == items.constEnd();
    if (flag)
        this->leftRightHorizontal(std::get<1>(graphicsModule).at(0), mousePointF);
    return !flag;
}

void GraphicsScene::updateLines(std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule, const std::array<std::tuple<QPointF, Direct>, 2> &coms) noexcept
{
    const std::tuple<QPointF, Direct> startCom = coms.at(0);
    const std::tuple<QPointF, Direct> endCom = coms.at(1);
    //水平或垂直
    if (std::get<0>(startCom).y() == std::get<0>(endCom).y() || std::get<0>(startCom).x() == std::get<0>(endCom).x()) {
        std::get<2>(graphicsModule).at(0)->setLine(QLineF(std::get<0>(startCom), std::get<0>(endCom)));
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
    } else if (std::get<0>(startCom).x() > std::get<0>(endCom).x()) {
        switch (std::get<1>(startCom)) {
            //起始元件节点方向朝左
            case Direct::Left:
                switch (std::get<1>(endCom)) {
                    //所需连接的元件节点方向
                    case Direct::Left:
                        this->targetBuck(true, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Top:
                        //左下方朝x方向折，左上方朝y方向折
                        this->targetBuck(std::get<0>(startCom).y() < std::get<0>(endCom).y(), std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Right:
                        this->targetMidX(std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Bottom:
                        //左上方朝x方向折，左下方朝y方向折
                        this->targetBuck(std::get<0>(startCom).y() > std::get<0>(endCom).y(), std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                }
                break;
            case Direct::Top:
                switch (std::get<1>(endCom)) {
                    //所需连接的元件节点方向
                    case Direct::Left:
                        this->targetBuck(true, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Top:
                        //左下方朝x方向折，左上方朝y方向折
                        this->targetBuck(std::get<0>(startCom).y() < std::get<0>(endCom).y(), std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Right:
                        this->targetBuck(true, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Bottom:
                        this->targetMidX(std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                }
                break;
            //起始元件节点方向朝右
            case Direct::Right:
                switch (std::get<1>(endCom)) {
                    //所需连接的元件节点方向
                    case Direct::Left:
                        this->targetMidY(std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Top:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Right:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Bottom:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                }
                break;
            case Direct::Bottom:
                switch (std::get<1>(endCom)) {
                    //所需连接的元件节点方向
                    case Direct::Left:
                        this->targetBuck(true, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Top:
                        this->targetMidX(std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Right:
                        this->targetBuck(true, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Bottom:
                        //左下方朝y方向折，左上方朝x方向折
                        this->targetBuck(std::get<0>(startCom).y() > std::get<0>(endCom).y(), std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                }
                break;
        }
    //朝右
    } else {
        switch (std::get<1>(startCom)) {
            //起始元件节点方向朝左
            case Direct::Left:
                switch (std::get<1>(endCom)) {
                    //所需连接的元件节点方向
                    case Direct::Left:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Top:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Right:
                        this->targetMidY(std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Bottom:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                }
                break;
            case Direct::Top:
                switch (std::get<1>(endCom)) {
                    //所需连接的元件节点方向
                    case Direct::Left:
                        //右下方朝x方向折，右上方朝y方向折
                        this->targetBuck(std::get<0>(startCom).y() < std::get<0>(endCom).y(), std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Top:
                        //右下方朝y方向折，右上方朝x方向折
                        this->targetBuck(std::get<0>(startCom).y() > std::get<0>(endCom).y(), std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Right:
                        this->targetBuck(true, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Bottom:
                        this->targetMidX(std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                }
                break;
            //起始元件节点方向朝右
            case Direct::Right:
                switch (std::get<1>(endCom)) {
                    //所需连接的元件节点方向
                    case Direct::Left:
                        this->targetMidX(std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Top:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Right:
                        this->targetBuck(true, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Bottom:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                }
                break;
            case Direct::Bottom:
                switch (std::get<1>(endCom)) {
                    //所需连接的元件节点方向
                    case Direct::Left:
                        //右下方朝y方向折，右上方朝x方向折
                        this->targetBuck(std::get<0>(startCom).y() > std::get<0>(endCom).y(), std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Top:
                        this->targetMidX(std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Right:
                        this->targetBuck(false, std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                    case Direct::Bottom:
                        //右下方朝x方向折，右上方朝y方向折
                        this->targetBuck(std::get<0>(startCom).y() < std::get<0>(endCom).y(), std::get<2>(graphicsModule), {std::get<0>(startCom), std::get<0>(endCom)});
                        break;
                }
                break;
        }
    }
    return;
}

void GraphicsScene::targetBuck(bool isX, std::array<QGraphicsLineItem*, 3> &gmLines, const std::array<QPointF, 2> &comps) noexcept
{
    //转折点
    QPointF pf = isX ? QPointF(comps.at(1).x(), comps.at(0).y()) : QPointF(comps.at(0).x(), comps.at(1).y());
    gmLines.at(0)->setLine(QLineF(comps.at(0), pf));
    if (gmLines.at(1) != nullptr) {
        gmLines.at(1)->setLine(QLineF(pf, comps.at(1)));
    } else {
        gmLines.at(1) = new QGraphicsLineItem(QLineF(pf, comps.at(1)));
        this->addItem(gmLines.at(1));
    }
    if (gmLines.at(2) != nullptr) {
        this->removeItem(gmLines.at(2));
        delete gmLines.at(2);
        gmLines.at(2) = nullptr;
    }
    return;
}

void GraphicsScene::targetMidX(std::array<QGraphicsLineItem*, 3> &gmLines, const std::array<QPointF, 2> &comps) noexcept
{
    QPointF startPos = comps.at(0);
    QPointF endPos = comps.at(1);
    double midX = endPos.x() < startPos.x() ? endPos.x() + abs(startPos.x() - endPos.x()) / 2 : endPos.x() - abs(startPos.x() - endPos.x()) / 2;
    gmLines.at(0)->setLine(QLineF(startPos, QPointF(midX, startPos.y())));
    if (gmLines.at(1) != nullptr) {
        gmLines.at(1)->setLine(midX, startPos.y(), midX, endPos.y());
    } else {
        gmLines.at(1) = new QGraphicsLineItem(midX, startPos.y(), midX, endPos.y());
        this->addItem(gmLines.at(1));
    }
    if (gmLines.at(2) != nullptr) {
        gmLines.at(2)->setLine(QLineF(QPointF(midX, endPos.y()), endPos));
    } else {
        gmLines.at(2) = new QGraphicsLineItem(QLineF(QPointF(midX, endPos.y()), endPos));
        this->addItem(gmLines.at(2));
    }
    return;
}

void GraphicsScene::targetMidY(std::array<QGraphicsLineItem*, 3> &gmLines, const std::array<QPointF, 2> &comps) noexcept
{
    QPointF startPos = comps.at(0);
    QPointF endPos = comps.at(1);
    double midY = endPos.y() < startPos.y() ? endPos.y() + abs(startPos.y() - endPos.y()) / 2 : endPos.y() - abs(startPos.y() - endPos.y()) / 2;
    gmLines.at(0)->setLine(QLineF(startPos, QPointF(startPos.x(), midY)));
    if (gmLines.at(1) != nullptr) {
        gmLines.at(1)->setLine(startPos.x(), midY, endPos.x(), midY);
    } else {
        gmLines.at(1) = new QGraphicsLineItem(startPos.x(), midY, endPos.y(), midY);
        this->addItem(gmLines.at(1));
    }
    if (gmLines.at(2) != nullptr) {
        gmLines.at(2)->setLine(QLineF(QPointF(endPos.x(), midY), endPos));
    } else {
        gmLines.at(2) = new QGraphicsLineItem(QLineF(QPointF(endPos.x(), midY), endPos));
        this->addItem(gmLines.at(2));
    }
    return;
}


void GraphicsScene::leftRightHorizontal(Direct direct, const QPointF &mousePointF) noexcept
{
    std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule = this->graphicsModules.back();
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
