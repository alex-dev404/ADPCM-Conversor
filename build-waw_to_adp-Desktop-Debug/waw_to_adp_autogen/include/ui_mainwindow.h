/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_4;
    QPushButton *btn_load;
    QListView *listView;
    QPushButton *btn_output;
    QLabel *label;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QProgressBar *progressBar;
    QPushButton *btn_converter;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(684, 394);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(10, 9, 661, 211));
        verticalLayout_4 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        btn_load = new QPushButton(verticalLayoutWidget);
        btn_load->setObjectName("btn_load");

        verticalLayout_4->addWidget(btn_load);

        listView = new QListView(verticalLayoutWidget);
        listView->setObjectName("listView");

        verticalLayout_4->addWidget(listView);

        btn_output = new QPushButton(verticalLayoutWidget);
        btn_output->setObjectName("btn_output");

        verticalLayout_4->addWidget(btn_output);

        label = new QLabel(verticalLayoutWidget);
        label->setObjectName("label");

        verticalLayout_4->addWidget(label);

        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(10, 240, 661, 27));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        progressBar = new QProgressBar(horizontalLayoutWidget);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(24);

        horizontalLayout->addWidget(progressBar);

        btn_converter = new QPushButton(horizontalLayoutWidget);
        btn_converter->setObjectName("btn_converter");

        horizontalLayout->addWidget(btn_converter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 684, 22));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btn_load->setText(QCoreApplication::translate("MainWindow", "Selecione os arquivos", nullptr));
        btn_output->setText(QCoreApplication::translate("MainWindow", "Selecione o diretorio de Saida", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Saida:", nullptr));
        btn_converter->setText(QCoreApplication::translate("MainWindow", "Converter", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
