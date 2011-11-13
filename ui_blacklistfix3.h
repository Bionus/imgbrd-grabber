/********************************************************************************
** Form generated from reading UI file 'blacklistfix3.ui'
**
** Created: Mon 14. Nov 00:07:07 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BLACKLISTFIX3_H
#define UI_BLACKLISTFIX3_H

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
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_BlacklistFix3
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTableWidget *tableWidget;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *buttonSelectBlacklisted;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;

    void setupUi(QDialog *BlacklistFix3)
    {
        if (BlacklistFix3->objectName().isEmpty())
            BlacklistFix3->setObjectName(QString::fromUtf8("BlacklistFix3"));
        BlacklistFix3->resize(600, 400);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        BlacklistFix3->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(BlacklistFix3);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(BlacklistFix3);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        tableWidget = new QTableWidget(BlacklistFix3);
        if (tableWidget->columnCount() < 4)
            tableWidget->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->horizontalHeader()->setHighlightSections(false);
        tableWidget->verticalHeader()->setDefaultSectionSize(50);
        tableWidget->verticalHeader()->setHighlightSections(false);

        verticalLayout->addWidget(tableWidget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        buttonSelectBlacklisted = new QPushButton(BlacklistFix3);
        buttonSelectBlacklisted->setObjectName(QString::fromUtf8("buttonSelectBlacklisted"));

        horizontalLayout_4->addWidget(buttonSelectBlacklisted);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        buttonOk = new QPushButton(BlacklistFix3);
        buttonOk->setObjectName(QString::fromUtf8("buttonOk"));

        horizontalLayout_4->addWidget(buttonOk);

        buttonCancel = new QPushButton(BlacklistFix3);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout_4->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(BlacklistFix3);

        QMetaObject::connectSlotsByName(BlacklistFix3);
    } // setupUi

    void retranslateUi(QDialog *BlacklistFix3)
    {
        BlacklistFix3->setWindowTitle(QApplication::translate("BlacklistFix3", "R\303\251parateur de liste noire", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BlacklistFix3", "Choisissez les images \303\240 supprimer dans la liste ci-dessous.", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem->setText(QApplication::translate("BlacklistFix3", "Miniature", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem1->setText(QApplication::translate("BlacklistFix3", "Nom", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem2->setText(QApplication::translate("BlacklistFix3", "Tag", 0, QApplication::UnicodeUTF8));
        buttonSelectBlacklisted->setText(QApplication::translate("BlacklistFix3", "S\303\251lectionner les images trouv\303\251es", 0, QApplication::UnicodeUTF8));
        buttonOk->setText(QApplication::translate("BlacklistFix3", "Ok", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("BlacklistFix3", "Annuler", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BlacklistFix3: public Ui_BlacklistFix3 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BLACKLISTFIX3_H
