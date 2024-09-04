#include "widget.h"

#include <QApplication>
#include<QSystemTrayIcon>
#include<QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    QSystemTrayIcon sysTray(QIcon("://img/icon.png"),&w);

    QMenu menu;
    auto showAct=new QAction("show",&sysTray);
    auto exitAct=new QAction("exit",&sysTray);

    QObject::connect(showAct,&QAction::triggered,[&](){
        w.setVisible(true);
    });
    QObject::connect(exitAct,&QAction::triggered,[&](){
        QApplication::quit();
    });

    menu.addAction(showAct);
    menu.addAction(exitAct);
    // QAction 对象不仅可以放在 QMenu 中，还可以放在 QToolBar、QMenuBar 和其他支持操作的控件中。
    // QAction 的作用是定义一个可执行的动作，实际显示的位置和形式取决于你将其添加到哪个控件中。例如，工具栏中的 QAction 可以显示为按钮，而菜单中的 QAction 显示为菜单项。

    sysTray.setContextMenu(&menu);
    // setContextMenu() 是 QSystemTrayIcon 特有的成员函数，用于设置右键点击托盘图标时显示的上下文菜单。其他控件如 QMenuBar 或 QToolBar 有各自的方法来处理菜单项的显示和交互。

    sysTray.show();
    w.show();
    return a.exec();
}
