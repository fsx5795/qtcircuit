#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = Q_NULLPTR);
    virtual ~GraphicsView(void);
protected:
    virtual void contextMenuEvent(QContextMenuEvent*) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
};

#endif