#include <QDebug>
#include <QGraphicsRectItem>
#include "graphicsscene.h"
void GraphicsScene::addRectItem(const QRect &rect)
{
    QGraphicsRectItem *rectItem = new QGraphicsRectItem(rect);
    rectItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    rectItem->setBrush(Qt::red);
    this->addItem(rectItem);
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
    vector<QLineF> lineFvec = this->rect_connect_line(startRectItem, endRectItem);
    //将连线添加到结构体
    this->change_lines(&module.lineList, lineFvec);
    this->moduleList.push_back(module);
    return;
}
//两个矩形图元之间连线
vector<QLineF> GraphicsScene::rect_connect_line(QGraphicsRectItem *startRectItem, QGraphicsRectItem *endRectItem)
{
    QRectF itemRect1 = startRectItem->sceneBoundingRect();
    QRectF itemRect2 = endRectItem->sceneBoundingRect();
    qreal startX = itemRect1.x() + itemRect1.width() + 1;
    qreal startY = itemRect1.topRight().y() + itemRect1.height() / 2;
    qreal endX = itemRect2.x() - 1;
    qreal endY = itemRect2.topRight().y() + itemRect2.height() / 2;
    vector<QLineF> lineFvec;
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
void GraphicsScene::change_lines(list<QGraphicsLineItem*> *lineList, const vector<QLineF> &lineFvec)
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
    vector<QLineF> lineFvec = this->rect_connect_line(module.startRect, module.endRect);
    //更新原有线段为新的连线线段
    this->change_lines(&module.lineList, lineFvec);
    return;
}
//删除多余的线段
void GraphicsScene::delete_lines(list<QGraphicsLineItem*> *lineList, int count)
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
void GraphicsScene::create_line(list<QGraphicsLineItem*> *lineList, const QLineF &lineF)
{
    QGraphicsLineItem *line = new QGraphicsLineItem(lineF);
    this->addItem(line);
    lineList->push_back(line);
    return;
}
void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    for (auto &item : this->items()) {
        if (item->type() == QGraphicsRectItem::Type && item->isSelected()) {
            for (auto &module : this->moduleList) {
                if (module.startRect == item || module.endRect == item) {
                    //更新连线
                    this->change_lines(module);
                }
            }
        }
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    return;
}
GraphicsScene::~GraphicsScene(void){}