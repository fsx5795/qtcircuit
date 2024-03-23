#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
using namespace std;
class GraphicsScene : public QGraphicsScene
{
    struct GraphicsModule
    {
        QGraphicsRectItem *startRect;
        list<QGraphicsLineItem*> lineList;
        QGraphicsRectItem *endRect;
    };
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;
    void addRectItem(const QRect&);
    virtual ~GraphicsScene(void);
protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent*) Q_DECL_OVERRIDE;
private:
    //新建立的矩形图元与上一个个矩形图元之间连线
    void new_rect_connect_line(QGraphicsRectItem*, QGraphicsRectItem*);
    //两个矩形图元之间连线
    vector<QLineF> rect_connect_line(QGraphicsRectItem*, QGraphicsRectItem*);
    //更新连线的线段
    void change_lines(list<QGraphicsLineItem*>*, const vector<QLineF>&);
    void change_lines(GraphicsModule&);
    //删除多余的线段
    void delete_lines(list<QGraphicsLineItem*>*, int);
    //添加缺少的线段
    void create_line(list<QGraphicsLineItem*>*, const QLineF&);
    list<GraphicsModule> moduleList;
};

#endif