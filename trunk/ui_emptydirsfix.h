/********************************************************************************
** Form generated from reading UI file 'emptydirsfix.ui'
**
** Created: Sat 26. Nov 23:33:40 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EMPTYDIRSFIX_H
#define UI_EMPTYDIRSFIX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_EmptyDirsFix
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *lineFolder;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonContinue;
    QPushButton *buttonCancel;

    void setupUi(QDialog *EmptyDirsFix)
    {
        if (EmptyDirsFix->objectName().isEmpty())
            EmptyDirsFix->setObjectName(QString::fromUtf8("EmptyDirsFix"));
        EmptyDirsFix->resize(274, 69);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        EmptyDirsFix->setWindowIcon(icon);
        formLayout = new QFormLayout(EmptyDirsFix);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(EmptyDirsFix);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        lineFolder = new QLineEdit(EmptyDirsFix);
        lineFolder->setObjectName(QString::fromUtf8("lineFolder"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lineFolder);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        buttonContinue = new QPushButton(EmptyDirsFix);
        buttonContinue->setObjectName(QString::fromUtf8("buttonContinue"));

        horizontalLayout_4->addWidget(buttonContinue);

        buttonCancel = new QPushButton(EmptyDirsFix);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout_4->addWidget(buttonCancel);


        formLayout->setLayout(1, QFormLayout::SpanningRole, horizontalLayout_4);


        retranslateUi(EmptyDirsFix);
        QObject::connect(buttonCancel, SIGNAL(clicked()), EmptyDirsFix, SLOT(reject()));
        QObject::connect(buttonContinue, SIGNAL(clicked()), EmptyDirsFix, SLOT(next()));

        QMetaObject::connectSlotsByName(EmptyDirsFix);
    } // setupUi

    void retranslateUi(QDialog *EmptyDirsFix)
    {
        EmptyDirsFix->setWindowTitle(QApplication::translate("EmptyDirsFix", "R\303\251parateur de dossiers vides", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("EmptyDirsFix", "Dossier", 0, QApplication::UnicodeUTF8));
        buttonContinue->setText(QApplication::translate("EmptyDirsFix", "Continuer", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("EmptyDirsFix", "Annuler", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class EmptyDirsFix: public Ui_EmptyDirsFix {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EMPTYDIRSFIX_H
