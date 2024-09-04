#include "widget.h"
#include<QPaintEvent>
#include<QPainter>//绘图
#include<QPixmap>//图片
#include<QCursor>
#include<QMetaEnum>


Widget::Widget(QWidget *parent)
    : QWidget(parent),
    timer(new QTimer(this)),
    menu(new QMenu(this))
{
    this->setWindowFlag(Qt::FramelessWindowHint);//去除窗口边框
    this->setAttribute(Qt::WA_TranslucentBackground);//背景透明

    this->installEventFilter(new DragFilter);

    connect(timer,&QTimer::timeout,[this](){
        static int index=0;//记录显示动作的当前图片索引
        auto paths = this->action_map.value(this->cur_role_act);
        this->cur_role_pix=paths[index++ % paths.size()];
        //paintEvent() 不允许的
        this->update();

    });

    initMenu();

    loadRoleActRes();

    showActAnimation(RoleAct::Swing);
}

Widget::~Widget() {}

void Widget::showActAnimation(RoleAct k)
{
    timer->stop();

    this->cur_role_act=k;

    timer->start(60);
}

void Widget::onMenuTriggered(QAction *action)
{
    QMetaEnum me = QMetaEnum::fromType<RoleAct>();

    bool ok;
    int k =  me.keyToValue(action->text().toStdString().c_str(),&ok);
    if(!ok)
      return;
     //获取点击的对象并且展示指定动画

    showActAnimation(static_cast<RoleAct>(k));
}

void Widget::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);

    QPixmap pix;
    pix.load(this->cur_role_pix.toLocalFile());
    //toLocalFile() 是 QUrl 类的一个成员函数，用于将 QUrl 对象转换为本地文件路径（QString 类型）。这个方法返回表示本地文件路径的 QString。

    painter.drawPixmap(0,0,pix);

}

// 如果窗口上有多个 QMenu，你可以在 contextMenuEvent() 中根据上下文选择哪个菜单弹出。可以通过逻辑判断或根据具体的触发位置来决定弹出哪个菜单。
void Widget::contextMenuEvent(QContextMenuEvent *event)
{
    this->menu->popup(QCursor::pos());
    // contextMenuEvent() 是一个用于处理右键点击事件的方法，它是 QWidget 类的虚函数，可以被子类重写来定制右键菜单的行为。你的代码示例如下：

    //         cpp
    //     void Widget::contextMenuEvent(QContextMenuEvent *event)
    // {
    //     this->menu->popup(QCursor::pos());
    // }
    // 这个函数的作用是当用户在 Widget 对象上点击右键时，弹出一个上下文菜单。具体说明如下：

    //         contextMenuEvent(QContextMenuEvent *event)：

    //     这是 QWidget 的虚函数，用于处理右键点击事件。当用户在 Widget 对象上右键点击时，这个函数会被调用。
    //     this->menu->popup(QCursor::pos());：

    //     this->menu 是一个 QMenu 对象的指针，表示要弹出的上下文菜单。
    //     popup() 方法用于显示 QMenu。QCursor::pos() 获取当前鼠标光标的位置，作为菜单弹出的坐标。

}

void Widget::loadRoleActRes()
{
    auto addRes=[this](RoleAct k,QString path,int count)
    {
        QList<QUrl> paths;
        char buf[260];
        for (int i = 0; i < count; ++i) {
            memset(buf, 0,sizeof(buf));
            sprintf_s(buf,path.toStdString().c_str(),i);

// path.toStdString():

// 1.toStdString() 是 QString 类的一个成员函数，用于将 QString 对象转换为 C++ 标准库的 std::string 对象。这是因为 QString 和 std::string 是不同的字符串类型，前者是 Qt 的字符串类，后者是 C++ 标准库的字符串类。
// path.toStdString().c_str():

// 2.c_str() 是 std::string 类的一个成员函数，返回一个 C 风格的字符串（即 const char*）。这个 C 风格字符串可以被 sprintf_s 等需要 C 风格字符串的函数使用。
// sprintf_s(buf, path.toStdString().c_str(), i);

// 3.buf 是一个字符数组，用于存储格式化后的字符串。
// path.toStdString().c_str() 返回 path 的 C 风格字符串表示，用于作为格式字符串传递给 sprintf_s。
// i 是一个格式化参数，它会根据 format 字符串插入到 buf 中。

            paths.append(QUrl::fromLocalFile(buf));
        //     QUrl::fromLocalFile(buf)
        //     是 Qt 库中的一个静态函数，用于将本地文件路径转换为 QUrl 对象。buf 是包含文件路径的字符串（通常是 QString 类型）。
        //     这个函数的作用是创建一个 QUrl 对象，该对象表示指定的本地文件路径，适用于在 Qt 应用程序中处理文件和路径。
        }
        action_map.insert(k,paths);
    };

    addRes(RoleAct::SayHello,":/sayHello/img/sayHello/sayHello_%d.png",28);
    addRes(RoleAct::Swing,":/swing/img/swing/swing_%d.png",32);
    addRes(RoleAct::Sleep,":/sleep/img/sleep/sleep_%d.png",25);
}

void Widget::initMenu()
{
    menu->addAction("SayHello");
    menu->addAction("Sleep");
    menu->addAction("Swing");

    QAction* act=new QAction("Hide");
    connect(act,&QAction::triggered,[this](){
        this->setVisible(false);
    });

    menu->addAction(act);

    connect(this->menu,&QMenu::triggered,this,&Widget::onMenuTriggered);
}
