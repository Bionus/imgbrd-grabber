/********************************************************************************
** Form generated from reading UI file 'detailsWindow.ui'
**
** Created: Sun 15. May 23:37:39 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DETAILSWINDOW_H
#define UI_DETAILSWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_detailsWindow
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *labelFormTags;
    QLabel *labelTags;
    QSpacerItem *verticalSpacer;
    QLabel *labelFormId;
    QLabel *labelId;
    QLabel *labelFormRating;
    QLabel *labelRating;
    QLabel *labelFormScore;
    QLabel *labelScore;
    QLabel *labelFormUser;
    QLabel *labelUser;
    QSpacerItem *verticalSpacer_2;
    QLabel *labelFormDate;
    QLabel *labelDate;
    QLabel *labelFormSize;
    QLabel *labelSize;
    QLabel *labelFormFilesize;
    QLabel *labelFilesize;
    QPushButton *pushButton;

    void setupUi(QWidget *detailsWindow)
    {
        if (detailsWindow->objectName().isEmpty())
            detailsWindow->setObjectName(QString::fromUtf8("detailsWindow"));
        detailsWindow->resize(400, 230);
        verticalLayout = new QVBoxLayout(detailsWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        labelFormTags = new QLabel(detailsWindow);
        labelFormTags->setObjectName(QString::fromUtf8("labelFormTags"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelFormTags);

        labelTags = new QLabel(detailsWindow);
        labelTags->setObjectName(QString::fromUtf8("labelTags"));
        labelTags->setContextMenuPolicy(Qt::CustomContextMenu);
        labelTags->setWordWrap(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, labelTags);

        verticalSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(1, QFormLayout::SpanningRole, verticalSpacer);

        labelFormId = new QLabel(detailsWindow);
        labelFormId->setObjectName(QString::fromUtf8("labelFormId"));

        formLayout->setWidget(2, QFormLayout::LabelRole, labelFormId);

        labelId = new QLabel(detailsWindow);
        labelId->setObjectName(QString::fromUtf8("labelId"));

        formLayout->setWidget(2, QFormLayout::FieldRole, labelId);

        labelFormRating = new QLabel(detailsWindow);
        labelFormRating->setObjectName(QString::fromUtf8("labelFormRating"));

        formLayout->setWidget(3, QFormLayout::LabelRole, labelFormRating);

        labelRating = new QLabel(detailsWindow);
        labelRating->setObjectName(QString::fromUtf8("labelRating"));
        labelRating->setWordWrap(true);

        formLayout->setWidget(3, QFormLayout::FieldRole, labelRating);

        labelFormScore = new QLabel(detailsWindow);
        labelFormScore->setObjectName(QString::fromUtf8("labelFormScore"));

        formLayout->setWidget(4, QFormLayout::LabelRole, labelFormScore);

        labelScore = new QLabel(detailsWindow);
        labelScore->setObjectName(QString::fromUtf8("labelScore"));
        labelScore->setWordWrap(true);

        formLayout->setWidget(4, QFormLayout::FieldRole, labelScore);

        labelFormUser = new QLabel(detailsWindow);
        labelFormUser->setObjectName(QString::fromUtf8("labelFormUser"));

        formLayout->setWidget(5, QFormLayout::LabelRole, labelFormUser);

        labelUser = new QLabel(detailsWindow);
        labelUser->setObjectName(QString::fromUtf8("labelUser"));
        labelUser->setWordWrap(true);

        formLayout->setWidget(5, QFormLayout::FieldRole, labelUser);

        verticalSpacer_2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(6, QFormLayout::SpanningRole, verticalSpacer_2);

        labelFormDate = new QLabel(detailsWindow);
        labelFormDate->setObjectName(QString::fromUtf8("labelFormDate"));

        formLayout->setWidget(7, QFormLayout::LabelRole, labelFormDate);

        labelDate = new QLabel(detailsWindow);
        labelDate->setObjectName(QString::fromUtf8("labelDate"));

        formLayout->setWidget(7, QFormLayout::FieldRole, labelDate);

        labelFormSize = new QLabel(detailsWindow);
        labelFormSize->setObjectName(QString::fromUtf8("labelFormSize"));

        formLayout->setWidget(8, QFormLayout::LabelRole, labelFormSize);

        labelSize = new QLabel(detailsWindow);
        labelSize->setObjectName(QString::fromUtf8("labelSize"));

        formLayout->setWidget(8, QFormLayout::FieldRole, labelSize);

        labelFormFilesize = new QLabel(detailsWindow);
        labelFormFilesize->setObjectName(QString::fromUtf8("labelFormFilesize"));

        formLayout->setWidget(9, QFormLayout::LabelRole, labelFormFilesize);

        labelFilesize = new QLabel(detailsWindow);
        labelFilesize->setObjectName(QString::fromUtf8("labelFilesize"));

        formLayout->setWidget(9, QFormLayout::FieldRole, labelFilesize);


        verticalLayout->addLayout(formLayout);

        pushButton = new QPushButton(detailsWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);


        retranslateUi(detailsWindow);
        QObject::connect(pushButton, SIGNAL(clicked()), detailsWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(detailsWindow);
    } // setupUi

    void retranslateUi(QWidget *detailsWindow)
    {
        detailsWindow->setWindowTitle(QApplication::translate("detailsWindow", "Form", 0, QApplication::UnicodeUTF8));
        labelFormTags->setText(QApplication::translate("detailsWindow", "<b>Tags</b>", 0, QApplication::UnicodeUTF8));
        labelTags->setText(QApplication::translate("detailsWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">Inconnus</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelFormId->setText(QApplication::translate("detailsWindow", "<b>ID</b>", 0, QApplication::UnicodeUTF8));
        labelId->setText(QApplication::translate("detailsWindow", "<i>Inconnu</i>", 0, QApplication::UnicodeUTF8));
        labelFormRating->setText(QApplication::translate("detailsWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Classe</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelRating->setText(QApplication::translate("detailsWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">Inconnue</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelFormScore->setText(QApplication::translate("detailsWindow", "<b>Score</b>", 0, QApplication::UnicodeUTF8));
        labelScore->setText(QApplication::translate("detailsWindow", "<i>Inconnu</i>", 0, QApplication::UnicodeUTF8));
        labelFormUser->setText(QApplication::translate("detailsWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Posteur</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelUser->setText(QApplication::translate("detailsWindow", "<i>Inconnu</i>", 0, QApplication::UnicodeUTF8));
        labelFormDate->setText(QApplication::translate("detailsWindow", "<b>Date<.b>", 0, QApplication::UnicodeUTF8));
        labelDate->setText(QApplication::translate("detailsWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">Inconnue</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelFormSize->setText(QApplication::translate("detailsWindow", "<b>Dimensions</b>", 0, QApplication::UnicodeUTF8));
        labelSize->setText(QApplication::translate("detailsWindow", "<i>Inconnues</i>", 0, QApplication::UnicodeUTF8));
        labelFormFilesize->setText(QApplication::translate("detailsWindow", "<b>Taille</b>", 0, QApplication::UnicodeUTF8));
        labelFilesize->setText(QApplication::translate("detailsWindow", "<i>Inconnue</i>", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("detailsWindow", "Fermer", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class detailsWindow: public Ui_detailsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DETAILSWINDOW_H
