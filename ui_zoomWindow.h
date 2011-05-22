/********************************************************************************
** Form generated from reading UI file 'zoomWindow.ui'
**
** Created: Sun 22. May 22:01:48 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZOOMWINDOW_H
#define UI_ZOOMWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_zoomWindow
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *labelFormTags;
    QLabel *labelTags;
    QPushButton *buttonDetails;
    QHBoxLayout *buttonsLayout;
    QPushButton *buttonSave;
    QPushButton *buttonSaveNQuit;
    QPushButton *buttonOpen;
    QPushButton *buttonSaveAs;

    void setupUi(QWidget *zoomWindow)
    {
        if (zoomWindow->objectName().isEmpty())
            zoomWindow->setObjectName(QString::fromUtf8("zoomWindow"));
        zoomWindow->resize(800, 600);
        zoomWindow->setBaseSize(QSize(0, 0));
        zoomWindow->setContextMenuPolicy(Qt::DefaultContextMenu);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        zoomWindow->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(zoomWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelFormTags = new QLabel(zoomWindow);
        labelFormTags->setObjectName(QString::fromUtf8("labelFormTags"));

        horizontalLayout->addWidget(labelFormTags);

        labelTags = new QLabel(zoomWindow);
        labelTags->setObjectName(QString::fromUtf8("labelTags"));
        labelTags->setContextMenuPolicy(Qt::CustomContextMenu);
        labelTags->setWordWrap(true);

        horizontalLayout->addWidget(labelTags);

        buttonDetails = new QPushButton(zoomWindow);
        buttonDetails->setObjectName(QString::fromUtf8("buttonDetails"));

        horizontalLayout->addWidget(buttonDetails);

        horizontalLayout->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout);

        buttonsLayout = new QHBoxLayout();
        buttonsLayout->setObjectName(QString::fromUtf8("buttonsLayout"));
        buttonSave = new QPushButton(zoomWindow);
        buttonSave->setObjectName(QString::fromUtf8("buttonSave"));

        buttonsLayout->addWidget(buttonSave);

        buttonSaveNQuit = new QPushButton(zoomWindow);
        buttonSaveNQuit->setObjectName(QString::fromUtf8("buttonSaveNQuit"));

        buttonsLayout->addWidget(buttonSaveNQuit);

        buttonOpen = new QPushButton(zoomWindow);
        buttonOpen->setObjectName(QString::fromUtf8("buttonOpen"));

        buttonsLayout->addWidget(buttonOpen);

        buttonSaveAs = new QPushButton(zoomWindow);
        buttonSaveAs->setObjectName(QString::fromUtf8("buttonSaveAs"));

        buttonsLayout->addWidget(buttonSaveAs);


        verticalLayout->addLayout(buttonsLayout);


        retranslateUi(zoomWindow);
        QObject::connect(buttonSave, SIGNAL(clicked()), zoomWindow, SLOT(saveImage()));
        QObject::connect(buttonSaveNQuit, SIGNAL(clicked()), zoomWindow, SLOT(saveNQuit()));
        QObject::connect(buttonOpen, SIGNAL(clicked()), zoomWindow, SLOT(openSaveDir()));
        QObject::connect(buttonSaveAs, SIGNAL(clicked()), zoomWindow, SLOT(saveImageAs()));
        QObject::connect(labelTags, SIGNAL(customContextMenuRequested(QPoint)), zoomWindow, SLOT(contextMenu(QPoint)));
        QObject::connect(labelTags, SIGNAL(linkHovered(QString)), zoomWindow, SLOT(linkHovered(QString)));
        QObject::connect(labelTags, SIGNAL(linkActivated(QString)), zoomWindow, SLOT(openUrl(QString)));

        QMetaObject::connectSlotsByName(zoomWindow);
    } // setupUi

    void retranslateUi(QWidget *zoomWindow)
    {
        zoomWindow->setWindowTitle(QApplication::translate("zoomWindow", "Grabber - Image", 0, QApplication::UnicodeUTF8));
        labelFormTags->setText(QApplication::translate("zoomWindow", "<b>Tags</b>", 0, QApplication::UnicodeUTF8));
        labelTags->setText(QString());
        buttonDetails->setText(QApplication::translate("zoomWindow", "Plus d'infos", 0, QApplication::UnicodeUTF8));
        buttonSave->setText(QApplication::translate("zoomWindow", "Enregistrer", 0, QApplication::UnicodeUTF8));
        buttonSaveNQuit->setText(QApplication::translate("zoomWindow", "Enregistrer et fermer", 0, QApplication::UnicodeUTF8));
        buttonOpen->setText(QApplication::translate("zoomWindow", "Ouvrir le dossier de destination", 0, QApplication::UnicodeUTF8));
        buttonSaveAs->setText(QApplication::translate("zoomWindow", "Enregistrer sous...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class zoomWindow: public Ui_zoomWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZOOMWINDOW_H
