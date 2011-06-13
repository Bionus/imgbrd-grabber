/********************************************************************************
** Form generated from reading UI file 'batchwindow.ui'
**
** Created: Mon 13. Jun 02:10:58 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATCHWINDOW_H
#define UI_BATCHWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_batchWindow
{
public:
    QGridLayout *gridLayout;
    QProgressBar *progressBar;
    QDialogButtonBox *buttonBox;
    QPushButton *buttonDetails;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout;
    QLabel *labelLog;
    QLabel *labelMessage;

    void setupUi(QDialog *batchWindow)
    {
        if (batchWindow->objectName().isEmpty())
            batchWindow->setObjectName(QString::fromUtf8("batchWindow"));
        batchWindow->resize(300, 164);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(batchWindow->sizePolicy().hasHeightForWidth());
        batchWindow->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        batchWindow->setWindowIcon(icon);
        batchWindow->setModal(true);
        gridLayout = new QGridLayout(batchWindow);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        progressBar = new QProgressBar(batchWindow);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);

        gridLayout->addWidget(progressBar, 1, 0, 1, 1);

        buttonBox = new QDialogButtonBox(batchWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 2);

        buttonDetails = new QPushButton(batchWindow);
        buttonDetails->setObjectName(QString::fromUtf8("buttonDetails"));
        buttonDetails->setCheckable(true);

        gridLayout->addWidget(buttonDetails, 1, 1, 1, 1);

        scrollArea = new QScrollArea(batchWindow);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 282, 69));
        QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy2);
        verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        labelLog = new QLabel(scrollAreaWidgetContents);
        labelLog->setObjectName(QString::fromUtf8("labelLog"));
        labelLog->setEnabled(true);
        sizePolicy2.setHeightForWidth(labelLog->sizePolicy().hasHeightForWidth());
        labelLog->setSizePolicy(sizePolicy2);
        labelLog->setTextFormat(Qt::RichText);
        labelLog->setOpenExternalLinks(true);

        verticalLayout->addWidget(labelLog);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 2, 0, 1, 2);

        labelMessage = new QLabel(batchWindow);
        labelMessage->setObjectName(QString::fromUtf8("labelMessage"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(labelMessage->sizePolicy().hasHeightForWidth());
        labelMessage->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(labelMessage, 0, 0, 1, 2);


        retranslateUi(batchWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), batchWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), batchWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(batchWindow);
    } // setupUi

    void retranslateUi(QDialog *batchWindow)
    {
        batchWindow->setWindowTitle(QApplication::translate("batchWindow", "Grabber - T\303\251l\303\251chargement group\303\251", 0, QApplication::UnicodeUTF8));
        buttonDetails->setText(QApplication::translate("batchWindow", "D\303\251tails", 0, QApplication::UnicodeUTF8));
        labelLog->setText(QString());
        labelMessage->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class batchWindow: public Ui_batchWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATCHWINDOW_H
