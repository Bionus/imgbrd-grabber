/********************************************************************************
** Form generated from reading UI file 'emptydirsfix2.ui'
**
** Created: Sat 26. Nov 23:48:40 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EMPTYDIRSFIX2_H
#define UI_EMPTYDIRSFIX2_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EmptyDirsFix2
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QListWidget *listWidget;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonDelete;
    QPushButton *buttonCancel;

    void setupUi(QDialog *EmptyDirsFix2)
    {
        if (EmptyDirsFix2->objectName().isEmpty())
            EmptyDirsFix2->setObjectName(QString::fromUtf8("EmptyDirsFix2"));
        EmptyDirsFix2->resize(395, 455);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        EmptyDirsFix2->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(EmptyDirsFix2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(EmptyDirsFix2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        listWidget = new QListWidget(EmptyDirsFix2);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

        verticalLayout->addWidget(listWidget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        buttonDelete = new QPushButton(EmptyDirsFix2);
        buttonDelete->setObjectName(QString::fromUtf8("buttonDelete"));

        horizontalLayout_4->addWidget(buttonDelete);

        buttonCancel = new QPushButton(EmptyDirsFix2);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout_4->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(EmptyDirsFix2);
        QObject::connect(buttonCancel, SIGNAL(clicked()), EmptyDirsFix2, SLOT(reject()));
        QObject::connect(buttonDelete, SIGNAL(clicked()), EmptyDirsFix2, SLOT(deleteSel()));

        QMetaObject::connectSlotsByName(EmptyDirsFix2);
    } // setupUi

    void retranslateUi(QDialog *EmptyDirsFix2)
    {
        EmptyDirsFix2->setWindowTitle(QApplication::translate("EmptyDirsFix2", "R\303\251parateur de dossiers vides", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("EmptyDirsFix2", "Choisissez les dossiers \303\240 supprimer dans la liste ci-dessous.", 0, QApplication::UnicodeUTF8));
        buttonDelete->setText(QApplication::translate("EmptyDirsFix2", "Supprimer", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("EmptyDirsFix2", "Annuler", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class EmptyDirsFix2: public Ui_EmptyDirsFix2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EMPTYDIRSFIX2_H
