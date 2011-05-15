/********************************************************************************
** Form generated from reading UI file 'zoomWindow.ui'
**
** Created: Sun 15. May 14:34:09 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZOOMWINDOW_H
#define UI_ZOOMWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
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
    QFormLayout *formLayout;
    QLabel *labelFormTags;
    QLabel *labelTags;
    QLabel *labelFormRating;
    QLabel *labelRating;
    QLabel *labelFormScore;
    QLabel *labelFormUser;
    QLabel *labelScore;
    QLabel *labelUser;
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
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        zoomWindow->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(zoomWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        labelFormTags = new QLabel(zoomWindow);
        labelFormTags->setObjectName(QString::fromUtf8("labelFormTags"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelFormTags);

        labelTags = new QLabel(zoomWindow);
        labelTags->setObjectName(QString::fromUtf8("labelTags"));
        labelTags->setContextMenuPolicy(Qt::CustomContextMenu);
        labelTags->setWordWrap(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, labelTags);

        labelFormRating = new QLabel(zoomWindow);
        labelFormRating->setObjectName(QString::fromUtf8("labelFormRating"));

        formLayout->setWidget(1, QFormLayout::LabelRole, labelFormRating);

        labelRating = new QLabel(zoomWindow);
        labelRating->setObjectName(QString::fromUtf8("labelRating"));
        labelRating->setWordWrap(true);

        formLayout->setWidget(1, QFormLayout::FieldRole, labelRating);

        labelFormScore = new QLabel(zoomWindow);
        labelFormScore->setObjectName(QString::fromUtf8("labelFormScore"));

        formLayout->setWidget(2, QFormLayout::LabelRole, labelFormScore);

        labelFormUser = new QLabel(zoomWindow);
        labelFormUser->setObjectName(QString::fromUtf8("labelFormUser"));

        formLayout->setWidget(3, QFormLayout::LabelRole, labelFormUser);

        labelScore = new QLabel(zoomWindow);
        labelScore->setObjectName(QString::fromUtf8("labelScore"));
        labelScore->setWordWrap(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, labelScore);

        labelUser = new QLabel(zoomWindow);
        labelUser->setObjectName(QString::fromUtf8("labelUser"));
        labelUser->setWordWrap(true);

        formLayout->setWidget(3, QFormLayout::FieldRole, labelUser);


        verticalLayout->addLayout(formLayout);

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
        QObject::connect(labelTags, SIGNAL(linkHovered(QString)), zoomWindow, SLOT(linkHovered(QString)));
        QObject::connect(labelTags, SIGNAL(linkActivated(QString)), zoomWindow, SLOT(openUrl(QString)));
        QObject::connect(labelTags, SIGNAL(customContextMenuRequested(QPoint)), zoomWindow, SLOT(contextMenu()));

        QMetaObject::connectSlotsByName(zoomWindow);
    } // setupUi

    void retranslateUi(QWidget *zoomWindow)
    {
        zoomWindow->setWindowTitle(QApplication::translate("zoomWindow", "Grabber - Image", 0, QApplication::UnicodeUTF8));
        labelFormTags->setText(QApplication::translate("zoomWindow", "<b>Tags</b>", 0, QApplication::UnicodeUTF8));
        labelTags->setText(QString());
        labelFormRating->setText(QApplication::translate("zoomWindow", "<b>Rating</b>", 0, QApplication::UnicodeUTF8));
        labelRating->setText(QString());
        labelFormScore->setText(QApplication::translate("zoomWindow", "<b>Score</b>", 0, QApplication::UnicodeUTF8));
        labelFormUser->setText(QApplication::translate("zoomWindow", "<b>User</b>", 0, QApplication::UnicodeUTF8));
        labelScore->setText(QString());
        labelUser->setText(QString());
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
