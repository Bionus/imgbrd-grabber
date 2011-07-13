/********************************************************************************
** Form generated from reading UI file 'searchtab.ui'
**
** Created: Tue 12. Jul 21:25:48 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEARCHTAB_H
#define UI_SEARCHTAB_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_searchTab
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *layoutFields;
    QLabel *label;
    QPushButton *buttonCalendar;
    QSpinBox *spinPage;
    QPushButton *buttonTags;
    QPushButton *pushButton;
    QWidget *widgetPlus;
    QGridLayout *layoutPlus;
    QLabel *label_6;
    QLabel *label_7;
    QSpinBox *spinImagesPerPage;
    QSpinBox *spinColumns;
    QLabel *label_8;
    QHBoxLayout *layoutSourcesList;
    QGridLayout *layoutResults;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonFirstPage;
    QPushButton *buttonPreviousPage;
    QPushButton *buttonSourcesList;
    QCheckBox *checkMergeResults;
    QLabel *labelMergeResults;
    QPushButton *buttonGetpage;
    QPushButton *buttonNextPage;
    QPushButton *buttonLastPage;

    void setupUi(QWidget *searchTab)
    {
        if (searchTab->objectName().isEmpty())
            searchTab->setObjectName(QString::fromUtf8("searchTab"));
        searchTab->resize(798, 531);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchTab->sizePolicy().hasHeightForWidth());
        searchTab->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(searchTab);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        layoutFields = new QHBoxLayout();
        layoutFields->setSpacing(6);
        layoutFields->setObjectName(QString::fromUtf8("layoutFields"));
        label = new QLabel(searchTab);
        label->setObjectName(QString::fromUtf8("label"));

        layoutFields->addWidget(label);

        buttonCalendar = new QPushButton(searchTab);
        buttonCalendar->setObjectName(QString::fromUtf8("buttonCalendar"));

        layoutFields->addWidget(buttonCalendar);

        spinPage = new QSpinBox(searchTab);
        spinPage->setObjectName(QString::fromUtf8("spinPage"));
        spinPage->setMinimumSize(QSize(60, 0));
        spinPage->setMinimum(1);
        spinPage->setMaximum(1000);

        layoutFields->addWidget(spinPage);

        buttonTags = new QPushButton(searchTab);
        buttonTags->setObjectName(QString::fromUtf8("buttonTags"));

        layoutFields->addWidget(buttonTags);

        pushButton = new QPushButton(searchTab);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMaximumSize(QSize(30, 16777215));
        pushButton->setCheckable(true);

        layoutFields->addWidget(pushButton);


        verticalLayout->addLayout(layoutFields);

        widgetPlus = new QWidget(searchTab);
        widgetPlus->setObjectName(QString::fromUtf8("widgetPlus"));
        widgetPlus->setEnabled(true);
        layoutPlus = new QGridLayout(widgetPlus);
        layoutPlus->setContentsMargins(0, 0, 0, 0);
        layoutPlus->setObjectName(QString::fromUtf8("layoutPlus"));
        label_6 = new QLabel(widgetPlus);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        layoutPlus->addWidget(label_6, 0, 0, 1, 1);

        label_7 = new QLabel(widgetPlus);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        layoutPlus->addWidget(label_7, 0, 2, 1, 1);

        spinImagesPerPage = new QSpinBox(widgetPlus);
        spinImagesPerPage->setObjectName(QString::fromUtf8("spinImagesPerPage"));
        spinImagesPerPage->setMinimum(1);
        spinImagesPerPage->setMaximum(1000);
        spinImagesPerPage->setValue(20);

        layoutPlus->addWidget(spinImagesPerPage, 0, 1, 1, 1);

        spinColumns = new QSpinBox(widgetPlus);
        spinColumns->setObjectName(QString::fromUtf8("spinColumns"));
        spinColumns->setMinimum(1);
        spinColumns->setMaximum(10);

        layoutPlus->addWidget(spinColumns, 0, 3, 1, 1);

        label_8 = new QLabel(widgetPlus);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        layoutPlus->addWidget(label_8, 1, 0, 1, 1);

        layoutSourcesList = new QHBoxLayout();
        layoutSourcesList->setObjectName(QString::fromUtf8("layoutSourcesList"));

        layoutPlus->addLayout(layoutSourcesList, 2, 0, 1, 4);


        verticalLayout->addWidget(widgetPlus);

        layoutResults = new QGridLayout();
        layoutResults->setObjectName(QString::fromUtf8("layoutResults"));

        verticalLayout->addLayout(layoutResults);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonFirstPage = new QPushButton(searchTab);
        buttonFirstPage->setObjectName(QString::fromUtf8("buttonFirstPage"));
        buttonFirstPage->setEnabled(false);
        buttonFirstPage->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(buttonFirstPage);

        buttonPreviousPage = new QPushButton(searchTab);
        buttonPreviousPage->setObjectName(QString::fromUtf8("buttonPreviousPage"));
        buttonPreviousPage->setEnabled(false);
        buttonPreviousPage->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(buttonPreviousPage);

        buttonSourcesList = new QPushButton(searchTab);
        buttonSourcesList->setObjectName(QString::fromUtf8("buttonSourcesList"));

        horizontalLayout->addWidget(buttonSourcesList);

        checkMergeResults = new QCheckBox(searchTab);
        checkMergeResults->setObjectName(QString::fromUtf8("checkMergeResults"));
        checkMergeResults->setChecked(true);

        horizontalLayout->addWidget(checkMergeResults);

        labelMergeResults = new QLabel(searchTab);
        labelMergeResults->setObjectName(QString::fromUtf8("labelMergeResults"));

        horizontalLayout->addWidget(labelMergeResults);

        buttonGetpage = new QPushButton(searchTab);
        buttonGetpage->setObjectName(QString::fromUtf8("buttonGetpage"));

        horizontalLayout->addWidget(buttonGetpage);

        buttonNextPage = new QPushButton(searchTab);
        buttonNextPage->setObjectName(QString::fromUtf8("buttonNextPage"));
        buttonNextPage->setEnabled(false);
        buttonNextPage->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(buttonNextPage);

        buttonLastPage = new QPushButton(searchTab);
        buttonLastPage->setObjectName(QString::fromUtf8("buttonLastPage"));
        buttonLastPage->setEnabled(false);
        buttonLastPage->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(buttonLastPage);

        horizontalLayout->setStretch(2, 1);
        horizontalLayout->setStretch(4, 1);
        horizontalLayout->setStretch(5, 1);

        verticalLayout->addLayout(horizontalLayout);

        verticalLayout->setStretch(2, 1);

        retranslateUi(searchTab);
        QObject::connect(buttonTags, SIGNAL(clicked()), searchTab, SLOT(load()));
        QObject::connect(pushButton, SIGNAL(clicked(bool)), widgetPlus, SLOT(setVisible(bool)));
        QObject::connect(buttonSourcesList, SIGNAL(clicked()), searchTab, SLOT(openSourcesWindow()));
        QObject::connect(buttonGetpage, SIGNAL(clicked()), searchTab, SLOT(getPage()));

        QMetaObject::connectSlotsByName(searchTab);
    } // setupUi

    void retranslateUi(QWidget *searchTab)
    {
        searchTab->setWindowTitle(QApplication::translate("searchTab", "Nouvel onglet", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("searchTab", "Recherche", 0, QApplication::UnicodeUTF8));
        buttonCalendar->setText(QApplication::translate("searchTab", " Ins\303\251rer une date ", 0, QApplication::UnicodeUTF8));
        buttonTags->setText(QApplication::translate("searchTab", "Ok", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("searchTab", "+", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("searchTab", "Images par page", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("searchTab", "Nombre de colonnes", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("searchTab", "Post-filtrage", 0, QApplication::UnicodeUTF8));
        buttonFirstPage->setText(QApplication::translate("searchTab", "<<", 0, QApplication::UnicodeUTF8));
        buttonPreviousPage->setText(QApplication::translate("searchTab", "<", 0, QApplication::UnicodeUTF8));
        buttonSourcesList->setText(QApplication::translate("searchTab", "Sources", 0, QApplication::UnicodeUTF8));
        checkMergeResults->setText(QApplication::translate("searchTab", "Fusionner les r\303\251sultats", 0, QApplication::UnicodeUTF8));
        labelMergeResults->setText(QString());
        buttonGetpage->setText(QApplication::translate("searchTab", "Prendre cette page", 0, QApplication::UnicodeUTF8));
        buttonNextPage->setText(QApplication::translate("searchTab", ">", 0, QApplication::UnicodeUTF8));
        buttonLastPage->setText(QApplication::translate("searchTab", ">>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class searchTab: public Ui_searchTab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEARCHTAB_H
