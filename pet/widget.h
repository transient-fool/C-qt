#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QMap>
#include<QList>
#include<QUrl>
#include<QTimer>
#include<QEvent>
#include<QMouseEvent>
#include<QContextMenuEvent>
#include<QMenu>

class QPaintEvent;

namespace Act {
Q_NAMESPACE

enum RoleAct{
    Swing,
    Sleep,
    SayHello
};
Q_ENUM_NS(RoleAct)

}

using namespace  Act ;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public:
    void showActAnimation(RoleAct k);

public slots:
    void onMenuTriggered(QAction* action);

protected:
    void paintEvent(QPaintEvent* event)override;
    void contextMenuEvent(QContextMenuEvent* event);

private:
    void loadRoleActRes();
    void initMenu();

private:
    QMap<RoleAct,QList<QUrl>> action_map;
    QTimer* timer;
    RoleAct cur_role_act;
    QUrl cur_role_pix;
    QMenu* menu;
};

class DragFilter:public QObject{

public:
    bool eventFilter(QObject* obj,QEvent* event)
    {
        auto w=dynamic_cast<QWidget*>(obj);
        // if(!w)
        //     return false;

        if(event->type()==QEvent::MouseButtonPress)
        {
            auto e=dynamic_cast<QMouseEvent*>(event);
            if(e)
            {
                pos=e->pos();
            }
        }
        else if(event->type()==QEvent::MouseMove)
        {
            auto e=dynamic_cast<QMouseEvent*>(event);
            if(e)
            {
                if(e->buttons()&Qt::MouseButton::LeftButton)
                {
                    w->move(e->globalPosition().toPoint()-pos);
                }
            }
        }
        return QObject::eventFilter(obj,event);
    }

    // 事件过滤器的安装：

    //         你需要将这个事件过滤器安装到一个对象上（通常是一个窗口部件），以便它能捕获该对象的事件。
    //             鼠标按钮按下事件：

    //     if (event->type() == QEvent::MouseButtonPress) 检查事件类型是否为鼠标按钮按下。
    //         记录鼠标按下时的位置 (pos = e->pos();)。
    //     鼠标移动事件：

    //     if (event->type() == QEvent::MouseMove) 检查事件类型是否为鼠标移动。
    //     if (e->buttons() & Qt::MouseButton::LeftButton) 检查左键是否被按下。
    //     如果左键按下且鼠标移动，则计算新的窗口位置 (w->move(e->globalPosition().toPoint() - pos);)，从而实现拖动窗口的效果。
    //     继续事件处理：

    //     return QObject::eventFilter(obj, event); 确保事件在处理完你的自定义逻辑后，继续传递到其他可能的事件过滤器或目标对象。
    //     注意事项
    //         确保 pos 是类成员变量：pos 需要是类的成员变量，确保可以在鼠标按钮按下和鼠标移动事件中共享。
    //             使用 globalPosition() 替代 pos()：globalPosition() 是 Qt 6 引入的，适用于更精确的坐标处理。如果使用 Qt 5，可能需要用 e->globalPos()。
    //     示例代码
private:
    QPoint pos;
};

#endif // WIDGET_H
