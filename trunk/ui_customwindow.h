/********************************************************************************
** Form generated from reading UI file 'customwindow.ui'
**
** Created: Tue 8. Nov 22:42:36 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMWINDOW_H
#define UI_CUSTOMWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_customWindow
{
public:
    QFormLayout *formLayout;
    QLabel *label_3;
    QLabel *label;
    QLineEdit *lineName;
    QLabel *label_2;
    QTextEdit *textTags;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *customWindow)
    {
        if (customWindow->objectName().isEmpty())
            customWindow->setObjectName(QString::fromUtf8("customWindow"));
        customWindow->setWindowModality(Qt::WindowModal);
        customWindow->resize(363, 189);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        customWindow->setWindowIcon(icon);
        formLayout = new QFormLayout(customWindow);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_3 = new QLabel(customWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::SpanningRole, label_3);

        label = new QLabel(customWindow);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        lineName = new QLineEdit(customWindow);
        lineName->setObjectName(QString::fromUtf8("lineName"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lineName);

        label_2 = new QLabel(customWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        textTags = new QTextEdit(customWindow);
        textTags->setObjectName(QString::fromUtf8("textTags"));

        formLayout->setWidget(2, QFormLayout::FieldRole, textTags);

        buttonBox = new QDialogButtonBox(customWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        formLayout->setWidget(3, QFormLayout::SpanningRole, buttonBox);


        retranslateUi(customWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), customWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), customWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(customWindow);
    } // setupUi

    void retranslateUi(QDialog *customWindow)
    {
        customWindow->setWindowTitle(QApplication::translate("customWindow", "Grabber - Ajouter un symbole personnalis\303\251", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("customWindow", "<i>S\303\251parer les tags par des espaces ou des sauts de ligne</i>", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("customWindow", "Nom", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("customWindow", "Tags", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class customWindow: public Ui_customWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMWINDOW_H
