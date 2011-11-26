/********************************************************************************
** Form generated from reading UI file 'searchwindow.ui'
**
** Created: Sat 26. Nov 15:46:25 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEARCHWINDOW_H
#define UI_SEARCHWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_SearchWindow
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QComboBox *comboOrder;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *comboRating;
    QLabel *label_4;
    QComboBox *comboStatus;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineDate;
    QPushButton *buttonCalendar;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *buttonImage;
    QDialogButtonBox *buttonBox;
    QSpacerItem *verticalSpacer;
    QLabel *label_5;

    void setupUi(QDialog *SearchWindow)
    {
        if (SearchWindow->objectName().isEmpty())
            SearchWindow->setObjectName(QString::fromUtf8("SearchWindow"));
        SearchWindow->resize(280, 215);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SearchWindow->sizePolicy().hasHeightForWidth());
        SearchWindow->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        SearchWindow->setWindowIcon(icon);
        formLayout = new QFormLayout(SearchWindow);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label = new QLabel(SearchWindow);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        comboOrder = new QComboBox(SearchWindow);
        comboOrder->setObjectName(QString::fromUtf8("comboOrder"));

        formLayout->setWidget(0, QFormLayout::FieldRole, comboOrder);

        label_2 = new QLabel(SearchWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        comboRating = new QComboBox(SearchWindow);
        comboRating->setObjectName(QString::fromUtf8("comboRating"));

        horizontalLayout_3->addWidget(comboRating);

        horizontalLayout_3->setStretch(0, 1);

        formLayout->setLayout(1, QFormLayout::FieldRole, horizontalLayout_3);

        label_4 = new QLabel(SearchWindow);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_4);

        comboStatus = new QComboBox(SearchWindow);
        comboStatus->setObjectName(QString::fromUtf8("comboStatus"));

        formLayout->setWidget(2, QFormLayout::FieldRole, comboStatus);

        label_3 = new QLabel(SearchWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineDate = new QLineEdit(SearchWindow);
        lineDate->setObjectName(QString::fromUtf8("lineDate"));

        horizontalLayout->addWidget(lineDate);

        buttonCalendar = new QPushButton(SearchWindow);
        buttonCalendar->setObjectName(QString::fromUtf8("buttonCalendar"));

        horizontalLayout->addWidget(buttonCalendar);


        formLayout->setLayout(3, QFormLayout::FieldRole, horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        buttonImage = new QPushButton(SearchWindow);
        buttonImage->setObjectName(QString::fromUtf8("buttonImage"));

        horizontalLayout_2->addWidget(buttonImage);

        buttonBox = new QDialogButtonBox(SearchWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_2->addWidget(buttonBox);


        formLayout->setLayout(6, QFormLayout::SpanningRole, horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(5, QFormLayout::SpanningRole, verticalSpacer);

        label_5 = new QLabel(SearchWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setWordWrap(true);

        formLayout->setWidget(4, QFormLayout::SpanningRole, label_5);


        retranslateUi(SearchWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), SearchWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SearchWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(SearchWindow);
    } // setupUi

    void retranslateUi(QDialog *SearchWindow)
    {
        SearchWindow->setWindowTitle(QApplication::translate("SearchWindow", "Grabber - Recherche", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SearchWindow", "Trier par", 0, QApplication::UnicodeUTF8));
        comboOrder->clear();
        comboOrder->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("SearchWindow", "ID (croissant)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "ID (d\303\251croissant)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Score (croissant)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Score (d\303\251croissant)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "M\303\251gapixels (croissant)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "M\303\251gapixels (d\303\251croissant)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Taille de fichier", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Orientation paysage", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Orientation portrait", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Nombre de favoris", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Rang", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("SearchWindow", "Rating", 0, QApplication::UnicodeUTF8));
        comboRating->clear();
        comboRating->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("SearchWindow", "Safe", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Safe (non)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Questionable", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Questionable (non)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Explicit", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Explicit (non)", 0, QApplication::UnicodeUTF8)
        );
        label_4->setText(QApplication::translate("SearchWindow", "Statut", 0, QApplication::UnicodeUTF8));
        comboStatus->clear();
        comboStatus->insertItems(0, QStringList()
         << QString()
         << QApplication::translate("SearchWindow", "Supprim\303\251e", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Active", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "Signal\303\251e", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SearchWindow", "En attente", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("SearchWindow", "Date", 0, QApplication::UnicodeUTF8));
        buttonCalendar->setText(QApplication::translate("SearchWindow", "Calendrier", 0, QApplication::UnicodeUTF8));
        buttonImage->setText(QApplication::translate("SearchWindow", "Image", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("SearchWindow", "<i>N'oubliez pas que certains imageboards emp\303\252chent l'utilisation de plus d'un certain nombre de tags pour les membres non premiums.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SearchWindow: public Ui_SearchWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEARCHWINDOW_H
