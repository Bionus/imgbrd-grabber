/********************************************************************************
** Form generated from reading UI file 'aboutwindow.ui'
**
** Created: Thu 3. Nov 00:31:00 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTWINDOW_H
#define UI_ABOUTWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_aboutWindow
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QLabel *labelCurrent;
    QLabel *labelMessage;
    QLabel *label_5;

    void setupUi(QWidget *aboutWindow)
    {
        if (aboutWindow->objectName().isEmpty())
            aboutWindow->setObjectName(QString::fromUtf8("aboutWindow"));
        aboutWindow->resize(400, 190);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        aboutWindow->setWindowIcon(icon);
        horizontalLayout = new QHBoxLayout(aboutWindow);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(aboutWindow);
        label->setObjectName(QString::fromUtf8("label"));
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/icon.png")));

        horizontalLayout->addWidget(label);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_2 = new QLabel(aboutWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        labelCurrent = new QLabel(aboutWindow);
        labelCurrent->setObjectName(QString::fromUtf8("labelCurrent"));

        verticalLayout->addWidget(labelCurrent);

        labelMessage = new QLabel(aboutWindow);
        labelMessage->setObjectName(QString::fromUtf8("labelMessage"));

        verticalLayout->addWidget(labelMessage);

        label_5 = new QLabel(aboutWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setTextFormat(Qt::RichText);
        label_5->setWordWrap(true);
        label_5->setOpenExternalLinks(true);

        verticalLayout->addWidget(label_5);


        horizontalLayout->addLayout(verticalLayout);

        horizontalLayout->setStretch(1, 1);

        retranslateUi(aboutWindow);

        QMetaObject::connectSlotsByName(aboutWindow);
    } // setupUi

    void retranslateUi(QWidget *aboutWindow)
    {
        aboutWindow->setWindowTitle(QApplication::translate("aboutWindow", "\303\200 propos de Grabber", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        label_2->setText(QApplication::translate("aboutWindow", "<span style=\" font-size:30pt; font-weight:600; color:#505050;\">Grabber</p>", 0, QApplication::UnicodeUTF8));
        labelCurrent->setText(QString());
        labelMessage->setText(QString());
        label_5->setText(QApplication::translate("aboutWindow", "Grabber est une cr\303\251ation de Bionus.<br/>N'h\303\251sitez pas \303\240 visiter le <a href=\"http://code.google.com/p/imgbrd-grabber/\">site</a> pour rester \303\240 jour, ou r\303\251cup\303\251rer des fichiers de site ou des traductions.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class aboutWindow: public Ui_aboutWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTWINDOW_H
