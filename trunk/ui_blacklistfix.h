/********************************************************************************
** Form generated from reading UI file 'blacklistfix.ui'
**
** Created: Sat 26. Nov 23:18:37 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BLACKLISTFIX_H
#define UI_BLACKLISTFIX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_BlacklistFix
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *lineFolder;
    QLabel *label_2;
    QLineEdit *lineFilename;
    QLabel *label_3;
    QLineEdit *lineBlacklist;
    QLabel *label_4;
    QComboBox *comboSource;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonContinue;
    QPushButton *buttonCancel;

    void setupUi(QDialog *BlacklistFix)
    {
        if (BlacklistFix->objectName().isEmpty())
            BlacklistFix->setObjectName(QString::fromUtf8("BlacklistFix"));
        BlacklistFix->resize(395, 147);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        BlacklistFix->setWindowIcon(icon);
        formLayout = new QFormLayout(BlacklistFix);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(BlacklistFix);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        lineFolder = new QLineEdit(BlacklistFix);
        lineFolder->setObjectName(QString::fromUtf8("lineFolder"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lineFolder);

        label_2 = new QLabel(BlacklistFix);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        lineFilename = new QLineEdit(BlacklistFix);
        lineFilename->setObjectName(QString::fromUtf8("lineFilename"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lineFilename);

        label_3 = new QLabel(BlacklistFix);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        lineBlacklist = new QLineEdit(BlacklistFix);
        lineBlacklist->setObjectName(QString::fromUtf8("lineBlacklist"));

        formLayout->setWidget(2, QFormLayout::FieldRole, lineBlacklist);

        label_4 = new QLabel(BlacklistFix);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        comboSource = new QComboBox(BlacklistFix);
        comboSource->setObjectName(QString::fromUtf8("comboSource"));

        formLayout->setWidget(3, QFormLayout::FieldRole, comboSource);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        buttonContinue = new QPushButton(BlacklistFix);
        buttonContinue->setObjectName(QString::fromUtf8("buttonContinue"));

        horizontalLayout_4->addWidget(buttonContinue);

        buttonCancel = new QPushButton(BlacklistFix);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout_4->addWidget(buttonCancel);


        formLayout->setLayout(4, QFormLayout::SpanningRole, horizontalLayout_4);


        retranslateUi(BlacklistFix);

        QMetaObject::connectSlotsByName(BlacklistFix);
    } // setupUi

    void retranslateUi(QDialog *BlacklistFix)
    {
        BlacklistFix->setWindowTitle(QApplication::translate("BlacklistFix", "R\303\251parateur de liste noire", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BlacklistFix", "Dossier", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("BlacklistFix", "Noms de fichiers", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("BlacklistFix", "Liste noire", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("BlacklistFix", "Source", 0, QApplication::UnicodeUTF8));
        buttonContinue->setText(QApplication::translate("BlacklistFix", "Continuer", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("BlacklistFix", "Annuler", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BlacklistFix: public Ui_BlacklistFix {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BLACKLISTFIX_H
