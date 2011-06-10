/********************************************************************************
** Form generated from reading UI file 'detailsWindow.ui'
**
** Created: Sun 5. Jun 20:29:08 2011
**      by: Qt User Interface Compiler version 4.6.2
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
#include <QtGui/QHBoxLayout>
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
    QSpacerItem *verticalSpacer1;
    QLabel *labelFormId;
    QLabel *labelId;
    QLabel *labelFormRating;
    QLabel *labelRating;
    QLabel *labelFormScore;
    QLabel *labelScore;
    QLabel *labelFormUser;
    QLabel *labelUser;
    QSpacerItem *verticalSpacer2;
    QLabel *labelFormDate;
    QLabel *labelDate;
    QLabel *labelFormSize;
    QLabel *labelSize;
    QLabel *labelFormFilesize;
    QLabel *labelFilesize;
    QLabel *labelFormMd5;
    QLabel *labelMd5;
    QSpacerItem *verticalSpacer3;
    QLabel *labelFormUrl;
    QLabel *labelFormSource;
    QLabel *labelSource;
    QLabel *labelUrl;
    QLabel *labelFormSample;
    QLabel *labelFormPreview;
    QLabel *labelSample;
    QLabel *labelPreview;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QFormLayout *formLayout_2;
    QLabel *labelFormParent;
    QLabel *labelParent;
    QLabel *labelFormChildren;
    QLabel *labelChildren;
    QFormLayout *formLayout_3;
    QLabel *labelFormComments;
    QLabel *labelComments;
    QLabel *labelFormNotes;
    QLabel *labelNotes;
    QSpacerItem *verticalSpacer_2;
    QPushButton *pushButton;

    void setupUi(QWidget *detailsWindow)
    {
        if (detailsWindow->objectName().isEmpty())
            detailsWindow->setObjectName(QString::fromUtf8("detailsWindow"));
        detailsWindow->setWindowModality(Qt::WindowModal);
        detailsWindow->resize(400, 412);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(detailsWindow->sizePolicy().hasHeightForWidth());
        detailsWindow->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        detailsWindow->setWindowIcon(icon);
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

        verticalSpacer1 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        formLayout->setItem(1, QFormLayout::SpanningRole, verticalSpacer1);

        labelFormId = new QLabel(detailsWindow);
        labelFormId->setObjectName(QString::fromUtf8("labelFormId"));

        formLayout->setWidget(2, QFormLayout::LabelRole, labelFormId);

        labelId = new QLabel(detailsWindow);
        labelId->setObjectName(QString::fromUtf8("labelId"));

        formLayout->setWidget(2, QFormLayout::FieldRole, labelId);

        labelFormRating = new QLabel(detailsWindow);
        labelFormRating->setObjectName(QString::fromUtf8("labelFormRating"));

        formLayout->setWidget(4, QFormLayout::LabelRole, labelFormRating);

        labelRating = new QLabel(detailsWindow);
        labelRating->setObjectName(QString::fromUtf8("labelRating"));
        labelRating->setWordWrap(true);

        formLayout->setWidget(4, QFormLayout::FieldRole, labelRating);

        labelFormScore = new QLabel(detailsWindow);
        labelFormScore->setObjectName(QString::fromUtf8("labelFormScore"));

        formLayout->setWidget(5, QFormLayout::LabelRole, labelFormScore);

        labelScore = new QLabel(detailsWindow);
        labelScore->setObjectName(QString::fromUtf8("labelScore"));
        labelScore->setWordWrap(true);

        formLayout->setWidget(5, QFormLayout::FieldRole, labelScore);

        labelFormUser = new QLabel(detailsWindow);
        labelFormUser->setObjectName(QString::fromUtf8("labelFormUser"));

        formLayout->setWidget(6, QFormLayout::LabelRole, labelFormUser);

        labelUser = new QLabel(detailsWindow);
        labelUser->setObjectName(QString::fromUtf8("labelUser"));
        labelUser->setWordWrap(true);

        formLayout->setWidget(6, QFormLayout::FieldRole, labelUser);

        verticalSpacer2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        formLayout->setItem(7, QFormLayout::SpanningRole, verticalSpacer2);

        labelFormDate = new QLabel(detailsWindow);
        labelFormDate->setObjectName(QString::fromUtf8("labelFormDate"));

        formLayout->setWidget(8, QFormLayout::LabelRole, labelFormDate);

        labelDate = new QLabel(detailsWindow);
        labelDate->setObjectName(QString::fromUtf8("labelDate"));

        formLayout->setWidget(8, QFormLayout::FieldRole, labelDate);

        labelFormSize = new QLabel(detailsWindow);
        labelFormSize->setObjectName(QString::fromUtf8("labelFormSize"));

        formLayout->setWidget(9, QFormLayout::LabelRole, labelFormSize);

        labelSize = new QLabel(detailsWindow);
        labelSize->setObjectName(QString::fromUtf8("labelSize"));

        formLayout->setWidget(9, QFormLayout::FieldRole, labelSize);

        labelFormFilesize = new QLabel(detailsWindow);
        labelFormFilesize->setObjectName(QString::fromUtf8("labelFormFilesize"));

        formLayout->setWidget(10, QFormLayout::LabelRole, labelFormFilesize);

        labelFilesize = new QLabel(detailsWindow);
        labelFilesize->setObjectName(QString::fromUtf8("labelFilesize"));

        formLayout->setWidget(10, QFormLayout::FieldRole, labelFilesize);

        labelFormMd5 = new QLabel(detailsWindow);
        labelFormMd5->setObjectName(QString::fromUtf8("labelFormMd5"));

        formLayout->setWidget(3, QFormLayout::LabelRole, labelFormMd5);

        labelMd5 = new QLabel(detailsWindow);
        labelMd5->setObjectName(QString::fromUtf8("labelMd5"));

        formLayout->setWidget(3, QFormLayout::FieldRole, labelMd5);

        verticalSpacer3 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        formLayout->setItem(11, QFormLayout::SpanningRole, verticalSpacer3);

        labelFormUrl = new QLabel(detailsWindow);
        labelFormUrl->setObjectName(QString::fromUtf8("labelFormUrl"));

        formLayout->setWidget(12, QFormLayout::LabelRole, labelFormUrl);

        labelFormSource = new QLabel(detailsWindow);
        labelFormSource->setObjectName(QString::fromUtf8("labelFormSource"));

        formLayout->setWidget(13, QFormLayout::LabelRole, labelFormSource);

        labelSource = new QLabel(detailsWindow);
        labelSource->setObjectName(QString::fromUtf8("labelSource"));
        labelSource->setOpenExternalLinks(true);

        formLayout->setWidget(13, QFormLayout::FieldRole, labelSource);

        labelUrl = new QLabel(detailsWindow);
        labelUrl->setObjectName(QString::fromUtf8("labelUrl"));
        labelUrl->setOpenExternalLinks(true);

        formLayout->setWidget(12, QFormLayout::FieldRole, labelUrl);

        labelFormSample = new QLabel(detailsWindow);
        labelFormSample->setObjectName(QString::fromUtf8("labelFormSample"));

        formLayout->setWidget(14, QFormLayout::LabelRole, labelFormSample);

        labelFormPreview = new QLabel(detailsWindow);
        labelFormPreview->setObjectName(QString::fromUtf8("labelFormPreview"));

        formLayout->setWidget(15, QFormLayout::LabelRole, labelFormPreview);

        labelSample = new QLabel(detailsWindow);
        labelSample->setObjectName(QString::fromUtf8("labelSample"));
        labelSample->setOpenExternalLinks(true);

        formLayout->setWidget(14, QFormLayout::FieldRole, labelSample);

        labelPreview = new QLabel(detailsWindow);
        labelPreview->setObjectName(QString::fromUtf8("labelPreview"));
        labelPreview->setOpenExternalLinks(true);

        formLayout->setWidget(15, QFormLayout::FieldRole, labelPreview);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        labelFormParent = new QLabel(detailsWindow);
        labelFormParent->setObjectName(QString::fromUtf8("labelFormParent"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, labelFormParent);

        labelParent = new QLabel(detailsWindow);
        labelParent->setObjectName(QString::fromUtf8("labelParent"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, labelParent);

        labelFormChildren = new QLabel(detailsWindow);
        labelFormChildren->setObjectName(QString::fromUtf8("labelFormChildren"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, labelFormChildren);

        labelChildren = new QLabel(detailsWindow);
        labelChildren->setObjectName(QString::fromUtf8("labelChildren"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, labelChildren);


        horizontalLayout->addLayout(formLayout_2);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        labelFormComments = new QLabel(detailsWindow);
        labelFormComments->setObjectName(QString::fromUtf8("labelFormComments"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, labelFormComments);

        labelComments = new QLabel(detailsWindow);
        labelComments->setObjectName(QString::fromUtf8("labelComments"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, labelComments);

        labelFormNotes = new QLabel(detailsWindow);
        labelFormNotes->setObjectName(QString::fromUtf8("labelFormNotes"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, labelFormNotes);

        labelNotes = new QLabel(detailsWindow);
        labelNotes->setObjectName(QString::fromUtf8("labelNotes"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, labelNotes);


        horizontalLayout->addLayout(formLayout_3);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer_2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

        verticalLayout->addItem(verticalSpacer_2);

        pushButton = new QPushButton(detailsWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);


        retranslateUi(detailsWindow);
        QObject::connect(pushButton, SIGNAL(clicked()), detailsWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(detailsWindow);
    } // setupUi

    void retranslateUi(QWidget *detailsWindow)
    {
        detailsWindow->setWindowTitle(QApplication::translate("detailsWindow", "Grabber - D\303\251tails", 0, QApplication::UnicodeUTF8));
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
        labelFormMd5->setText(QApplication::translate("detailsWindow", "<b>MD5</b>", 0, QApplication::UnicodeUTF8));
        labelMd5->setText(QApplication::translate("detailsWindow", "<i>Inconnu</i>", 0, QApplication::UnicodeUTF8));
        labelFormUrl->setText(QApplication::translate("detailsWindow", "<b>URL</b>", 0, QApplication::UnicodeUTF8));
        labelFormSource->setText(QApplication::translate("detailsWindow", "<b>Source</b>", 0, QApplication::UnicodeUTF8));
        labelSource->setText(QApplication::translate("detailsWindow", "<i>Inconnue</i>", 0, QApplication::UnicodeUTF8));
        labelUrl->setText(QApplication::translate("detailsWindow", "<i>Inconnue</i>", 0, QApplication::UnicodeUTF8));
        labelFormSample->setText(QApplication::translate("detailsWindow", "<b>Sample</b>", 0, QApplication::UnicodeUTF8));
        labelFormPreview->setText(QApplication::translate("detailsWindow", "<b>Vignette</b>", 0, QApplication::UnicodeUTF8));
        labelSample->setText(QApplication::translate("detailsWindow", "<i>Inconnue</i>", 0, QApplication::UnicodeUTF8));
        labelPreview->setText(QApplication::translate("detailsWindow", "<i>Inconnue</i>", 0, QApplication::UnicodeUTF8));
        labelFormParent->setText(QApplication::translate("detailsWindow", "<b>Parent</b>", 0, QApplication::UnicodeUTF8));
        labelParent->setText(QApplication::translate("detailsWindow", "<i>Inconnu</i>", 0, QApplication::UnicodeUTF8));
        labelFormChildren->setText(QApplication::translate("detailsWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Enfants</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelChildren->setText(QApplication::translate("detailsWindow", "<i>Inconnu</i>", 0, QApplication::UnicodeUTF8));
        labelFormComments->setText(QApplication::translate("detailsWindow", "<b>Commentaires</b>", 0, QApplication::UnicodeUTF8));
        labelComments->setText(QApplication::translate("detailsWindow", "<i>Inconnu</i>", 0, QApplication::UnicodeUTF8));
        labelFormNotes->setText(QApplication::translate("detailsWindow", "<b>Notes</b>", 0, QApplication::UnicodeUTF8));
        labelNotes->setText(QApplication::translate("detailsWindow", "<i>Inconnu</i>", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("detailsWindow", "Fermer", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class detailsWindow: public Ui_detailsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DETAILSWINDOW_H
