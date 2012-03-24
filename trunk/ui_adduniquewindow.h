/********************************************************************************
** Form generated from reading UI file 'adduniquewindow.ui'
**
** Created: Fri 23. Mar 22:30:38 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDUNIQUEWINDOW_H
#define UI_ADDUNIQUEWINDOW_H

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

class Ui_AddUniqueWindow
{
public:
    QFormLayout *formLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QComboBox *comboSites;
    QLineEdit *lineId;
    QLineEdit *lineMd5;
    QSpacerItem *verticalSpacer;
    QLabel *label_4;
    QLineEdit *lineFilename;
    QLabel *labelFilename;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineFolder;
    QPushButton *buttonFolder;

    void setupUi(QDialog *AddUniqueWindow)
    {
        if (AddUniqueWindow->objectName().isEmpty())
            AddUniqueWindow->setObjectName(QString::fromUtf8("AddUniqueWindow"));
        AddUniqueWindow->resize(296, 220);
        formLayout = new QFormLayout(AddUniqueWindow);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton = new QPushButton(AddUniqueWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);

        buttonBox = new QDialogButtonBox(AddUniqueWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        formLayout->setLayout(8, QFormLayout::SpanningRole, horizontalLayout);

        label = new QLabel(AddUniqueWindow);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        label_2 = new QLabel(AddUniqueWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        label_3 = new QLabel(AddUniqueWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        comboSites = new QComboBox(AddUniqueWindow);
        comboSites->setObjectName(QString::fromUtf8("comboSites"));

        formLayout->setWidget(0, QFormLayout::FieldRole, comboSites);

        lineId = new QLineEdit(AddUniqueWindow);
        lineId->setObjectName(QString::fromUtf8("lineId"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lineId);

        lineMd5 = new QLineEdit(AddUniqueWindow);
        lineMd5->setObjectName(QString::fromUtf8("lineMd5"));

        formLayout->setWidget(2, QFormLayout::FieldRole, lineMd5);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(7, QFormLayout::LabelRole, verticalSpacer);

        label_4 = new QLabel(AddUniqueWindow);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_4);

        lineFilename = new QLineEdit(AddUniqueWindow);
        lineFilename->setObjectName(QString::fromUtf8("lineFilename"));

        formLayout->setWidget(4, QFormLayout::FieldRole, lineFilename);

        labelFilename = new QLabel(AddUniqueWindow);
        labelFilename->setObjectName(QString::fromUtf8("labelFilename"));

        formLayout->setWidget(5, QFormLayout::SpanningRole, labelFilename);

        label_5 = new QLabel(AddUniqueWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_5);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lineFolder = new QLineEdit(AddUniqueWindow);
        lineFolder->setObjectName(QString::fromUtf8("lineFolder"));

        horizontalLayout_2->addWidget(lineFolder);

        buttonFolder = new QPushButton(AddUniqueWindow);
        buttonFolder->setObjectName(QString::fromUtf8("buttonFolder"));

        horizontalLayout_2->addWidget(buttonFolder);


        formLayout->setLayout(3, QFormLayout::FieldRole, horizontalLayout_2);


        retranslateUi(AddUniqueWindow);
        QObject::connect(buttonBox, SIGNAL(rejected()), AddUniqueWindow, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), AddUniqueWindow, SLOT(ok()));
        QObject::connect(pushButton, SIGNAL(clicked()), AddUniqueWindow, SLOT(add()));

        QMetaObject::connectSlotsByName(AddUniqueWindow);
    } // setupUi

    void retranslateUi(QDialog *AddUniqueWindow)
    {
        AddUniqueWindow->setWindowTitle(QApplication::translate("AddUniqueWindow", "Grabber - Ajouter une image", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("AddUniqueWindow", "Ajouter", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AddUniqueWindow", "Site", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AddUniqueWindow", "Id", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("AddUniqueWindow", "Md5", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("AddUniqueWindow", "Nom de fichier", 0, QApplication::UnicodeUTF8));
        labelFilename->setText(QString());
        label_5->setText(QApplication::translate("AddUniqueWindow", "Dossier", 0, QApplication::UnicodeUTF8));
        buttonFolder->setText(QApplication::translate("AddUniqueWindow", "Parcourir", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AddUniqueWindow: public Ui_AddUniqueWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDUNIQUEWINDOW_H
