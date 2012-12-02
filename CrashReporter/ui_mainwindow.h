/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun 2. Dec 00:40:54 2012
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QCheckBox *checkSend;
    QWidget *widget;
    QFormLayout *formLayout;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_5;
    QLineEdit *lineLog;
    QLineEdit *lineSettings;
    QLineEdit *lineDump;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonRestart;
    QPushButton *buttonQuit;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(353, 240);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        verticalLayout->addWidget(label);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label_2->setWordWrap(true);

        verticalLayout->addWidget(label_2);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);
        label_3->setWordWrap(true);

        verticalLayout->addWidget(label_3);

        checkSend = new QCheckBox(centralWidget);
        checkSend->setObjectName(QString::fromUtf8("checkSend"));

        verticalLayout->addWidget(checkSend);

        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setEnabled(false);
        formLayout = new QFormLayout(widget);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(17, 0, 0, 0);
        label_4 = new QLabel(widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        label_6 = new QLabel(widget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_6);

        label_5 = new QLabel(widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        lineLog = new QLineEdit(widget);
        lineLog->setObjectName(QString::fromUtf8("lineLog"));
        lineLog->setReadOnly(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, lineLog);

        lineSettings = new QLineEdit(widget);
        lineSettings->setObjectName(QString::fromUtf8("lineSettings"));
        lineSettings->setReadOnly(true);

        formLayout->setWidget(1, QFormLayout::FieldRole, lineSettings);

        lineDump = new QLineEdit(widget);
        lineDump->setObjectName(QString::fromUtf8("lineDump"));
        lineDump->setReadOnly(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, lineDump);


        verticalLayout->addWidget(widget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonRestart = new QPushButton(centralWidget);
        buttonRestart->setObjectName(QString::fromUtf8("buttonRestart"));

        horizontalLayout->addWidget(buttonRestart);

        buttonQuit = new QPushButton(centralWidget);
        buttonQuit->setObjectName(QString::fromUtf8("buttonQuit"));

        horizontalLayout->addWidget(buttonQuit);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);
        QObject::connect(checkSend, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Grabber Crash Reporter", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "<b>D\303\251sol\303\251</b>", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Grabber a rencontr\303\251 un probl\303\250me et a plant\303\251. Le programme essaiera de r\303\251staurer vos onglets et autres param\303\250tres lors de son prochain d\303\251marrage.", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Pour nous aider \303\240 r\303\251soudre le probl\303\250me, vous pouvez nous envoyer un rapport de crash.", 0, QApplication::UnicodeUTF8));
        checkSend->setText(QApplication::translate("MainWindow", "Envoyer un rapport de crash", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Log", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Settings", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Dump", 0, QApplication::UnicodeUTF8));
        buttonRestart->setText(QApplication::translate("MainWindow", "Red\303\251marrer", 0, QApplication::UnicodeUTF8));
        buttonQuit->setText(QApplication::translate("MainWindow", "Quitter", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
