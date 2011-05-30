/********************************************************************************
** Form generated from reading UI file 'startwindow.ui'
**
** Created: Mon 30. May 21:28:03 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STARTWINDOW_H
#define UI_STARTWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_startWindow
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *welcomeLabel;
    QSpacerItem *verticalSpacer;
    QGridLayout *gridLayout;
    QLabel *pathLabel;
    QLineEdit *pathLineEdit;
    QPushButton *openButton;
    QLabel *filenameLabel;
    QLineEdit *filenameLineEdit;
    QLabel *validatorLabel;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *startWindow)
    {
        if (startWindow->objectName().isEmpty())
            startWindow->setObjectName(QString::fromUtf8("startWindow"));
        startWindow->setWindowModality(Qt::ApplicationModal);
        startWindow->resize(400, 200);
        verticalLayout = new QVBoxLayout(startWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        welcomeLabel = new QLabel(startWindow);
        welcomeLabel->setObjectName(QString::fromUtf8("welcomeLabel"));
        welcomeLabel->setTextFormat(Qt::AutoText);
        welcomeLabel->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        welcomeLabel->setWordWrap(true);

        verticalLayout->addWidget(welcomeLabel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pathLabel = new QLabel(startWindow);
        pathLabel->setObjectName(QString::fromUtf8("pathLabel"));

        gridLayout->addWidget(pathLabel, 0, 0, 1, 1);

        pathLineEdit = new QLineEdit(startWindow);
        pathLineEdit->setObjectName(QString::fromUtf8("pathLineEdit"));

        gridLayout->addWidget(pathLineEdit, 0, 1, 1, 1);

        openButton = new QPushButton(startWindow);
        openButton->setObjectName(QString::fromUtf8("openButton"));

        gridLayout->addWidget(openButton, 0, 2, 1, 1);

        filenameLabel = new QLabel(startWindow);
        filenameLabel->setObjectName(QString::fromUtf8("filenameLabel"));

        gridLayout->addWidget(filenameLabel, 1, 0, 1, 1);

        filenameLineEdit = new QLineEdit(startWindow);
        filenameLineEdit->setObjectName(QString::fromUtf8("filenameLineEdit"));

        gridLayout->addWidget(filenameLineEdit, 1, 1, 1, 2);

        gridLayout->setColumnStretch(1, 1);

        verticalLayout->addLayout(gridLayout);

        validatorLabel = new QLabel(startWindow);
        validatorLabel->setObjectName(QString::fromUtf8("validatorLabel"));
        validatorLabel->setTextFormat(Qt::RichText);
        validatorLabel->setWordWrap(true);

        verticalLayout->addWidget(validatorLabel);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        buttonBox = new QDialogButtonBox(startWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ignore|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(startWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), startWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), startWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(startWindow);
    } // setupUi

    void retranslateUi(QDialog *startWindow)
    {
        startWindow->setWindowTitle(QApplication::translate("startWindow", "Grabber - Premier lancement", 0, QApplication::UnicodeUTF8));
        welcomeLabel->setText(QApplication::translate("startWindow", "Avant de commencer, le programme a besoin de quelques informations n\303\251c\303\251ssaires \303\240 son bon fonctionnement. Vous pouvez ignorer cette \303\251tape, et les dites informations vous seront demand\303\251es en temps et en heure.", 0, QApplication::UnicodeUTF8));
        pathLabel->setText(QApplication::translate("startWindow", "Dossier de sauvegarde", 0, QApplication::UnicodeUTF8));
        openButton->setText(QApplication::translate("startWindow", "Parcourir", 0, QApplication::UnicodeUTF8));
        filenameLabel->setText(QApplication::translate("startWindow", "Noms des fichiers", 0, QApplication::UnicodeUTF8));
        validatorLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class startWindow: public Ui_startWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STARTWINDOW_H
