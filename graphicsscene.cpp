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
            std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule = this->graphicsMoudules.back();
			for (auto &item : this->items()) {
                if (item->type() == QGraphicsItemGroup::Type && std::get<0>(graphicsModule).at(0) != item) {
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
        } else {
			for (auto &item : this->items()) {
                if (item->type() == QGraphicsItemGroup::Type) {
                    std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> graphicsModule;
                    std::get<2>(graphicsModule).at(0) = nullptr;
                    std::get<2>(graphicsModule).at(1) = nullptr;
                    std::get<2>(graphicsModule).at(2) = nullptr;
                    std::array<std::tuple<QPointF, Direct>, 2> pd = static_cast<Component*>(item)->points();
                    if (sqrt((std::get<0>(pd.at(0)).x() - mouseEvent->scenePos().x()) + (std::get<0>(pd.at(0)).y() - mouseEvent->scenePos().y())) <= 10) {
                        std::get<2>(graphicsModule).at(0) = new QGraphicsLineItem(QLineF(std::get<0>(pd.at(0)), mouseEvent->scenePos()));
                        std::get<1>(graphicsModule).at(0) = std::get<1>(pd.at(0));
                    } else if(sqrt((std::get<0>(pd.at(1)).x() - mouseEvent->scenePos().x()) + (std::get<0>(pd.at(1)).y() - mouseEvent->scenePos().y())) <= 10) {
                        std::get<2>(graphicsModule).at(0) = new QGraphicsLineItem(QLineF(std::get<0>(pd.at(1)), mouseEvent->scenePos()));
                        std::get<1>(graphicsModule).at(0) = std::get<1>(pd.at(1));
                    }
                    if (std::get<2>(graphicsModule).at(0) != nullptr) {
                        this->graphicsMoudules.push_back(graphicsModule);
                        this->addItem(std::get<2>(graphicsModule).at(0));
                        this->drawEndLine = true;
                        this->views().at(0)->setMouseTracking(true);
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
        switch (std::get<1>(graphicsModule).at(0)) {
            case Direct::Left:
            case Direct::Right:
                this->leftRightHorizontal(std::get<1>(graphicsModule).at(0), mouseEvent->scenePos());
                break;
            default:
                break;
        }
    } else {
        qDebug() << "------------";
    }
    return QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void GraphicsScene::leftRightHorizontal(Direct direct, const QPointF &mousePointF) noexcept
{
    std::tuple<std::array<Component*, 2>, std::array<Direct, 2>, std::array<QGraphicsLineItem*, 3>> &graphicsModule = this->graphicsMoudules.back();
    QLineF frontLineF = std::get<2>(graphicsModule).at(0)->line();
    Bear bear;
    double midX;
    switch (direct)
    {
    case Direct::Left:
        bear = frontLineF.x1() > mousePointF.x() ? Bear::Positive : frontLineF.x1() < mousePointF.x() ? Bear::Reverse : Bear::Same, mousePointF;
        midX = mousePointF.x() + abs(frontLineF.x1() - mousePointF.x()) / 2;
        break;
    case Direct::Right:
        bear = frontLineF.x1() < mousePointF.x() ? Bear::Positive : frontLineF.x1() > mousePointF.x() ? Bear::Reverse : Bear::Same, mousePointF;
        midX = mousePointF.x() - abs(frontLineF.x1() - mousePointF.x()) / 2;
        break;
    default:
        break;
    }
    switch (bear) {
        case Bear::Positive:
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