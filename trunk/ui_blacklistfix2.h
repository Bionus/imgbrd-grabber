/********************************************************************************
** Form generated from reading UI file 'blacklistfix2.ui'
**
** Created: Mon 2. Jan 13:50:26 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BLACKLISTFIX2_H
#define UI_BLACKLISTFIX2_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_BlacklistFix2
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTreeWidget *treeWidget;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonContinue;
    QPushButton *buttonCancel;

    void setupUi(QDialog *BlacklistFix2)
    {
        if (BlacklistFix2->objectName().isEmpty())
            BlacklistFix2->setObjectName(QString::fromUtf8("BlacklistFix2"));
        BlacklistFix2->resize(395, 455);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        BlacklistFix2->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(BlacklistFix2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(BlacklistFix2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        treeWidget = new QTreeWidget(BlacklistFix2);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setSelectionMode(QAbstractItemView::MultiSelection);
        treeWidget->header()->setVisible(false);

        verticalLayout->addWidget(treeWidget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        buttonContinue = new QPushButton(BlacklistFix2);
        buttonContinue->setObjectName(QString::fromUtf8("buttonContinue"));

        horizontalLayout_4->addWidget(buttonContinue);

        buttonCancel = new QPushButton(BlacklistFix2);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout_4->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(BlacklistFix2);

        QMetaObject::connectSlotsByName(BlacklistFix2);
    } // setupUi

    void retranslateUi(QDialog *BlacklistFix2)
    {
        BlacklistFix2->setWindowTitle(QApplication::translate("BlacklistFix2", "R\303\251parateur de liste noire", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BlacklistFix2", "Choisissez les dossiers \303\240 tester dans la liste ci-dessous.", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("BlacklistFix2", "1", 0, QApplication::UnicodeUTF8));
        buttonContinue->setText(QApplication::translate("BlacklistFix2", "Continuer", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("BlacklistFix2", "Annuler", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BlacklistFix2: public Ui_BlacklistFix2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BLACKLISTFIX2_H
