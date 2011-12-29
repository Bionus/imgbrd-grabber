/********************************************************************************
** Form generated from reading UI file 'filenamewindow.ui'
**
** Created: Sat 17. Dec 23:25:00 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILENAMEWINDOW_H
#define UI_FILENAMEWINDOW_H

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

QT_BEGIN_NAMESPACE

class Ui_filenameWindow
{
public:
    QFormLayout *formLayout;
    QLabel *label_3;
    QLabel *label;
    QLineEdit *lineCondition;
    QLabel *label_2;
    QDialogButtonBox *buttonBox;
    QLineEdit *lineFilename;

    void setupUi(QDialog *filenameWindow)
    {
        if (filenameWindow->objectName().isEmpty())
            filenameWindow->setObjectName(QString::fromUtf8("filenameWindow"));
        filenameWindow->setWindowModality(Qt::WindowModal);
        filenameWindow->resize(363, 112);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        filenameWindow->setWindowIcon(icon);
        formLayout = new QFormLayout(filenameWindow);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_3 = new QLabel(filenameWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::SpanningRole, label_3);

        label = new QLabel(filenameWindow);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        lineCondition = new QLineEdit(filenameWindow);
        lineCondition->setObjectName(QString::fromUtf8("lineCondition"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lineCondition);

        label_2 = new QLabel(filenameWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        buttonBox = new QDialogButtonBox(filenameWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        formLayout->setWidget(3, QFormLayout::SpanningRole, buttonBox);

        lineFilename = new QLineEdit(filenameWindow);
        lineFilename->setObjectName(QString::fromUtf8("lineFilename"));

        formLayout->setWidget(2, QFormLayout::FieldRole, lineFilename);


        retranslateUi(filenameWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), filenameWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), filenameWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(filenameWindow);
    } // setupUi

    void retranslateUi(QDialog *filenameWindow)
    {
        filenameWindow->setWindowTitle(QApplication::translate("filenameWindow", "Grabber - Ajouter un symbole personnalis\303\251", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("filenameWindow", "<i>Vous pouvez utiliser un symbole ou un tag en tant que condition</i>", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("filenameWindow", "Condition", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("filenameWindow", "Nom de fichier", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class filenameWindow: public Ui_filenameWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILENAMEWINDOW_H
