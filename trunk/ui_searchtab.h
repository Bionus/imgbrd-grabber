/********************************************************************************
** Form generated from reading UI file 'searchtab.ui'
**
** Created: Mon 18. Jul 14:47:57 2011
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
#include <QtGui/QScrollArea>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_searchTab
{
public:
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QLabel *labelTags;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_4;
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
        verticalLayout_2 = new QVBoxLayout(searchTab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        splitter = new QSplitter(searchTab);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        scrollArea = new QScrollArea(splitter);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        sizePolicy.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setFrameShadow(QFrame::Plain);
        scrollArea->setLineWidth(0);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 758, 513));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy1);
        scrollAreaWidgetContents->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        labelTags = new QLabel(scrollAreaWidgetContents);
        labelTags->setObjectName(QString::fromUtf8("labelTags"));
        sizePolicy1.setHeightForWidth(labelTags->sizePolicy().hasHeightForWidth());
        labelTags->setSizePolicy(sizePolicy1);
        labelTags->setTextFormat(Qt::RichText);

        verticalLayout_3->addWidget(labelTags);

        scrollArea->setWidget(scrollAreaWidgetContents);
        splitter->addWidget(scrollArea);
        widget_2 = new QWidget(splitter);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        sizePolicy.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy);
        verticalLayout_4 = new QVBoxLayout(widget_2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        layoutFields = new QHBoxLayout();
        layoutFields->setSpacing(6);
        layoutFields->setObjectName(QString::fromUtf8("layoutFields"));
        label = new QLabel(widget_2);
        label->setObjectName(QString::fromUtf8("label"));

        layoutFields->addWidget(label);

        buttonCalendar = new QPushButton(widget_2);
        buttonCalendar->setObjectName(QString::fromUtf8("buttonCalendar"));

        layoutFields->addWidget(buttonCalendar);

        spinPage = new QSpinBox(widget_2);
        spinPage->setObjectName(QString::fromUtf8("spinPage"));
        spinPage->setMinimumSize(QSize(60, 0));
        spinPage->setMinimum(1);
        spinPage->setMaximum(1000);

        layoutFields->addWidget(spinPage);

        buttonTags = new QPushButton(widget_2);
        buttonTags->setObjectName(QString::fromUtf8("buttonTags"));

        layoutFields->addWidget(buttonTags);

        pushButton = new QPushButton(widget_2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMaximumSize(QSize(30, 16777215));
        pushButton->setCheckable(true);

        layoutFields->addWidget(pushButton);


        verticalLayout_4->addLayout(layoutFields);

        widgetPlus = new QWidget(widget_2);
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


        verticalLayout_4->addWidget(widgetPlus);

        layoutResults = new QGridLayout();
        layoutResults->setObjectName(QString::fromUtf8("layoutResults"));

        verticalLayout_4->addLayout(layoutResults);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonFirstPage = new QPushButton(widget_2);
        buttonFirstPage->setObjectName(QString::fromUtf8("buttonFirstPage"));
        buttonFirstPage->setEnabled(false);
        buttonFirstPage->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(buttonFirstPage);

        buttonPreviousPage = new QPushButton(widget_2);
        buttonPreviousPage->setObjectName(QString::fromUtf8("buttonPreviousPage"));
        buttonPreviousPage->setEnabled(false);
        buttonPreviousPage->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(buttonPreviousPage);

        buttonSourcesList = new QPushButton(widget_2);
        buttonSourcesList->setObjectName(QString::fromUtf8("buttonSourcesList"));

        horizontalLayout->addWidget(buttonSourcesList);

        checkMergeResults = new QCheckBox(widget_2);
        checkMergeResults->setObjectName(QString::fromUtf8("checkMergeResults"));
        checkMergeResults->setChecked(true);

        horizontalLayout->addWidget(checkMergeResults);

        labelMergeResults = new QLabel(widget_2);
        labelMergeResults->setObjectName(QString::fromUtf8("labelMergeResults"));

        horizontalLayout->addWidget(labelMergeResults);

        buttonGetpage = new QPushButton(widget_2);
        buttonGetpage->setObjectName(QString::fromUtf8("buttonGetpage"));

        horizontalLayout->addWidget(buttonGetpage);

        buttonNextPage = new QPushButton(widget_2);
        buttonNextPage->setObjectName(QString::fromUtf8("buttonNextPage"));
        buttonNextPage->setEnabled(false);
        buttonNextPage->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(buttonNextPage);

        buttonLastPage = new QPushButton(widget_2);
        buttonLastPage->setObjectName(QString::fromUtf8("buttonLastPage"));
        buttonLastPage->setEnabled(false);
        buttonLastPage->setMaximumSize(QSize(30, 16777215));

        horizontalLayout->addWidget(buttonLastPage);

        horizontalLayout->setStretch(2, 1);
        horizontalLayout->setStretch(4, 1);
        horizontalLayout->setStretch(5, 1);

        verticalLayout_4->addLayout(horizontalLayout);

        splitter->addWidget(widget_2);

        verticalLayout_2->addWidget(splitter);


        retranslateUi(searchTab);
        QObject::connect(buttonGetpage, SIGNAL(clicked()), searchTab, SLOT(getPage()));
        QObject::connect(pushButton, SIGNAL(clicked(bool)), widgetPlus, SLOT(setVisible(bool)));
        QObject::connect(buttonTags, SIGNAL(clicked()), searchTab, SLOT(load()));
        QObject::connect(buttonSourcesList, SIGNAL(clicked()), searchTab, SLOT(openSourcesWindow()));

        QMetaObject::connectSlotsByName(searchTab);
    } // setupUi

    void retranslateUi(QWidget *searchTab)
    {
        searchTab->setWindowTitle(QApplication::translate("searchTab", "Nouvel onglet", 0, QApplication::UnicodeUTF8));
        labelTags->setText(QString());
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
