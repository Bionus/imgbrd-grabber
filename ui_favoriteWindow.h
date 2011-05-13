/********************************************************************************
** Form generated from reading UI file 'favoriteWindow.ui'
**
** Created: Fri 13. May 23:31:47 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FAVORITEWINDOW_H
#define UI_FAVORITEWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDateTimeEdit>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_favoriteWindow
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout_2;
    QLabel *tagLabel;
    QLineEdit *tagLineEdit;
    QLabel *noteLabel;
    QSpinBox *noteSpinBox;
    QLabel *lastViewedLabel;
    QDateTimeEdit *lastViewedDateTimeEdit;
    QLabel *imageLabel;
    QHBoxLayout *horizontalLayout;
    QLineEdit *imageLineEdit;
    QPushButton *openButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *favoriteWindow)
    {
        if (favoriteWindow->objectName().isEmpty())
            favoriteWindow->setObjectName(QString::fromUtf8("favoriteWindow"));
        favoriteWindow->resize(304, 152);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(favoriteWindow->sizePolicy().hasHeightForWidth());
        favoriteWindow->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(favoriteWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        tagLabel = new QLabel(favoriteWindow);
        tagLabel->setObjectName(QString::fromUtf8("tagLabel"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, tagLabel);

        tagLineEdit = new QLineEdit(favoriteWindow);
        tagLineEdit->setObjectName(QString::fromUtf8("tagLineEdit"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, tagLineEdit);

        noteLabel = new QLabel(favoriteWindow);
        noteLabel->setObjectName(QString::fromUtf8("noteLabel"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, noteLabel);

        noteSpinBox = new QSpinBox(favoriteWindow);
        noteSpinBox->setObjectName(QString::fromUtf8("noteSpinBox"));
        noteSpinBox->setMaximum(100);
        noteSpinBox->setValue(50);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, noteSpinBox);

        lastViewedLabel = new QLabel(favoriteWindow);
        lastViewedLabel->setObjectName(QString::fromUtf8("lastViewedLabel"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, lastViewedLabel);

        lastViewedDateTimeEdit = new QDateTimeEdit(favoriteWindow);
        lastViewedDateTimeEdit->setObjectName(QString::fromUtf8("lastViewedDateTimeEdit"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, lastViewedDateTimeEdit);

        imageLabel = new QLabel(favoriteWindow);
        imageLabel->setObjectName(QString::fromUtf8("imageLabel"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, imageLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        imageLineEdit = new QLineEdit(favoriteWindow);
        imageLineEdit->setObjectName(QString::fromUtf8("imageLineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(imageLineEdit->sizePolicy().hasHeightForWidth());
        imageLineEdit->setSizePolicy(sizePolicy1);
        imageLineEdit->setReadOnly(false);

        horizontalLayout->addWidget(imageLineEdit);

        openButton = new QPushButton(favoriteWindow);
        openButton->setObjectName(QString::fromUtf8("openButton"));

        horizontalLayout->addWidget(openButton);

        horizontalLayout->setStretch(0, 1);

        formLayout_2->setLayout(3, QFormLayout::FieldRole, horizontalLayout);


        verticalLayout->addLayout(formLayout_2);

        buttonBox = new QDialogButtonBox(favoriteWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(favoriteWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), favoriteWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), favoriteWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(favoriteWindow);
    } // setupUi

    void retranslateUi(QDialog *favoriteWindow)
    {
        favoriteWindow->setWindowTitle(QApplication::translate("favoriteWindow", "Dialog", 0, QApplication::UnicodeUTF8));
        tagLabel->setText(QApplication::translate("favoriteWindow", "Tag", 0, QApplication::UnicodeUTF8));
        noteLabel->setText(QApplication::translate("favoriteWindow", "Note", 0, QApplication::UnicodeUTF8));
        noteSpinBox->setSuffix(QApplication::translate("favoriteWindow", " %", 0, QApplication::UnicodeUTF8));
        lastViewedLabel->setText(QApplication::translate("favoriteWindow", "Derni\303\250re vue", 0, QApplication::UnicodeUTF8));
        lastViewedDateTimeEdit->setDisplayFormat(QApplication::translate("favoriteWindow", "dd/MM/yyyy HH:mm:ss", 0, QApplication::UnicodeUTF8));
        imageLabel->setText(QApplication::translate("favoriteWindow", "Image", 0, QApplication::UnicodeUTF8));
        openButton->setText(QApplication::translate("favoriteWindow", "Parcourir", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class favoriteWindow: public Ui_favoriteWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FAVORITEWINDOW_H
