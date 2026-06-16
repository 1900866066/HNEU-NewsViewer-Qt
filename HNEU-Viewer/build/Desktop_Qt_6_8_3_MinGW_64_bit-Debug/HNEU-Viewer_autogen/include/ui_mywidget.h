/********************************************************************************
** Form generated from reading UI file 'mywidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYWIDGET_H
#define UI_MYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyWidget
{
public:
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_3;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_3;
    QTableWidget *tableWidget;

    void setupUi(QWidget *MyWidget)
    {
        if (MyWidget->objectName().isEmpty())
            MyWidget->setObjectName("MyWidget");
        MyWidget->resize(800, 600);
        verticalLayout_2 = new QVBoxLayout(MyWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        widget = new QWidget(MyWidget);
        widget->setObjectName("widget");
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        frame_3 = new QFrame(widget);
        frame_3->setObjectName("frame_3");
        frame_3->setAutoFillBackground(false);
        frame_3->setFrameShape(QFrame::Shape::WinPanel);
        frame_3->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_3);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label = new QLabel(frame_3);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        label_2 = new QLabel(frame_3);
        label_2->setObjectName("label_2");
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        label_2->setFont(font);

        horizontalLayout_2->addWidget(label_2);

        horizontalSpacer_3 = new QSpacerItem(630, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(frame_3);

        frame_2 = new QFrame(widget);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::Shape::Box);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_3 = new QLabel(frame_2);
        label_3->setObjectName("label_3");
        QFont font1;
        font1.setPointSize(11);
        font1.setBold(true);
        label_3->setFont(font1);

        horizontalLayout->addWidget(label_3);

        lineEdit = new QLineEdit(frame_2);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setMinimumSize(QSize(120, 35));
        QFont font2;
        font2.setPointSize(11);
        lineEdit->setFont(font2);
        lineEdit->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout->addWidget(lineEdit);

        pushButton = new QPushButton(frame_2);
        pushButton->setObjectName("pushButton");
        pushButton->setMinimumSize(QSize(150, 35));
        pushButton->setFont(font1);

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(frame_2);

        frame = new QFrame(widget);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Shape::Box);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_3 = new QHBoxLayout(frame);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        tableWidget = new QTableWidget(frame);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setFont(font1);
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setFont(font1);
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        __qtablewidgetitem2->setFont(font1);
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidget->setObjectName("tableWidget");
        QFont font3;
        font3.setPointSize(10);
        tableWidget->setFont(font3);
        tableWidget->setFrameShape(QFrame::Shape::Box);
        tableWidget->setFrameShadow(QFrame::Shadow::Plain);
        tableWidget->setTabKeyNavigation(false);
        tableWidget->setProperty("showDropIndicator", QVariant(true));
        tableWidget->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
        tableWidget->setTextElideMode(Qt::TextElideMode::ElideRight);
        tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);
        tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);
        tableWidget->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        tableWidget->horizontalHeader()->setStretchLastSection(false);

        horizontalLayout_3->addWidget(tableWidget);


        verticalLayout->addWidget(frame);


        verticalLayout_2->addWidget(widget);


        retranslateUi(MyWidget);

        QMetaObject::connectSlotsByName(MyWidget);
    } // setupUi

    void retranslateUi(QWidget *MyWidget)
    {
        MyWidget->setWindowTitle(QCoreApplication::translate("MyWidget", "\346\271\226\345\215\227\345\267\245\347\250\213\345\255\246\351\231\242\346\226\260\351\227\273\347\275\221\347\210\254\350\231\253", nullptr));
        label->setText(QCoreApplication::translate("MyWidget", "logo", nullptr));
        label_2->setText(QCoreApplication::translate("MyWidget", "\346\271\226\345\215\227\345\267\245\347\250\213\345\255\246\351\231\242\346\226\260\351\227\273\347\275\221\347\210\254\350\231\253", nullptr));
        label_3->setText(QCoreApplication::translate("MyWidget", "\347\210\254\345\217\226\346\235\241\346\225\260\357\274\232", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("MyWidget", "\350\257\267\350\276\223\345\205\245\346\225\260\351\207\217(1-100)", nullptr));
        pushButton->setText(QCoreApplication::translate("MyWidget", " \345\274\200\345\247\213\347\210\254\345\217\226", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MyWidget", "\345\272\217\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MyWidget", "\346\226\260\351\227\273\346\240\207\351\242\230", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MyWidget", "\345\217\221\345\270\203\346\227\266\351\227\264", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyWidget: public Ui_MyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYWIDGET_H
