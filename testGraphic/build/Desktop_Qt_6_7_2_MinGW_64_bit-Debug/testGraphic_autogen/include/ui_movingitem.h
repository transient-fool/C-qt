/********************************************************************************
** Form generated from reading UI file 'movingitem.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOVINGITEM_H
#define UI_MOVINGITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MovingItem
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MovingItem)
    {
        if (MovingItem->objectName().isEmpty())
            MovingItem->setObjectName("MovingItem");
        MovingItem->resize(800, 600);
        centralwidget = new QWidget(MovingItem);
        centralwidget->setObjectName("centralwidget");
        MovingItem->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MovingItem);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 18));
        MovingItem->setMenuBar(menubar);
        statusbar = new QStatusBar(MovingItem);
        statusbar->setObjectName("statusbar");
        MovingItem->setStatusBar(statusbar);

        retranslateUi(MovingItem);

        QMetaObject::connectSlotsByName(MovingItem);
    } // setupUi

    void retranslateUi(QMainWindow *MovingItem)
    {
        MovingItem->setWindowTitle(QCoreApplication::translate("MovingItem", "MovingItem", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MovingItem: public Ui_MovingItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOVINGITEM_H
