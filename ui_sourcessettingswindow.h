/********************************************************************************
** Form generated from reading UI file 'sourcessettingswindow.ui'
**
** Created: Sun 30. Dec 15:16:14 2012
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SOURCESSETTINGSWINDOW_H
#define UI_SOURCESSETTINGSWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SourcesSettingsWindow
{
public:
    QFormLayout *formLayout;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QFormLayout *formLayout_5;
    QLabel *label_7;
    QComboBox *comboReferer;
    QLabel *label_12;
    QLabel *label_13;
    QComboBox *comboRefererPreview;
    QComboBox *comboRefererImage;
    QWidget *tab;
    QFormLayout *formLayout_3;
    QWidget *widget;
    QFormLayout *formLayout_2;
    QLabel *label_3;
    QComboBox *comboSources1;
    QLabel *label_4;
    QComboBox *comboSources2;
    QLabel *label_5;
    QComboBox *comboSources3;
    QLabel *label_6;
    QComboBox *comboSources4;
    QCheckBox *checkSourcesDefault;
    QWidget *tab_2;
    QFormLayout *formLayout_4;
    QLabel *label;
    QLineEdit *lineAuthPseudo;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineAuthPassword;
    QPushButton *buttonAuthHash;
    QWidget *tab_4;
    QFormLayout *formLayout_7;
    QCheckBox *checkLoginParameter;
    QWidget *widget_2;
    QFormLayout *formLayout_6;
    QLabel *label_8;
    QComboBox *comboLoginMethod;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLineEdit *lineLoginUrl;
    QLineEdit *lineLoginPseudo;
    QLineEdit *lineLoginPassword;

    void setupUi(QDialog *SourcesSettingsWindow)
    {
        if (SourcesSettingsWindow->objectName().isEmpty())
            SourcesSettingsWindow->setObjectName(QString::fromUtf8("SourcesSettingsWindow"));
        SourcesSettingsWindow->resize(409, 215);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        SourcesSettingsWindow->setWindowIcon(icon);
        formLayout = new QFormLayout(SourcesSettingsWindow);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        horizontalLayout_4 = new QHBoxLayout();
#ifndef Q_OS_MAC
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
#endif
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        pushButton = new QPushButton(SourcesSettingsWindow);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout_4->addWidget(pushButton);

        pushButton_2 = new QPushButton(SourcesSettingsWindow);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        horizontalLayout_4->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(SourcesSettingsWindow);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout_4->addWidget(pushButton_3);


        formLayout->setLayout(1, QFormLayout::SpanningRole, horizontalLayout_4);

        tabWidget = new QTabWidget(SourcesSettingsWindow);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        formLayout_5 = new QFormLayout(tab_3);
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        label_7 = new QLabel(tab_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, label_7);

        comboReferer = new QComboBox(tab_3);
        comboReferer->setObjectName(QString::fromUtf8("comboReferer"));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, comboReferer);

        label_12 = new QLabel(tab_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_5->setWidget(1, QFormLayout::LabelRole, label_12);

        label_13 = new QLabel(tab_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        formLayout_5->setWidget(2, QFormLayout::LabelRole, label_13);

        comboRefererPreview = new QComboBox(tab_3);
        comboRefererPreview->setObjectName(QString::fromUtf8("comboRefererPreview"));

        formLayout_5->setWidget(1, QFormLayout::FieldRole, comboRefererPreview);

        comboRefererImage = new QComboBox(tab_3);
        comboRefererImage->setObjectName(QString::fromUtf8("comboRefererImage"));

        formLayout_5->setWidget(2, QFormLayout::FieldRole, comboRefererImage);

        tabWidget->addTab(tab_3, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        formLayout_3 = new QFormLayout(tab);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        widget = new QWidget(tab);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setEnabled(false);
        formLayout_2 = new QFormLayout(widget);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout_2->setContentsMargins(18, 0, 0, 0);
        label_3 = new QLabel(widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_3);

        comboSources1 = new QComboBox(widget);
        comboSources1->setObjectName(QString::fromUtf8("comboSources1"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboSources1);

        label_4 = new QLabel(widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_4);

        comboSources2 = new QComboBox(widget);
        comboSources2->setObjectName(QString::fromUtf8("comboSources2"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, comboSources2);

        label_5 = new QLabel(widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_5);

        comboSources3 = new QComboBox(widget);
        comboSources3->setObjectName(QString::fromUtf8("comboSources3"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, comboSources3);

        label_6 = new QLabel(widget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_6);

        comboSources4 = new QComboBox(widget);
        comboSources4->setObjectName(QString::fromUtf8("comboSources4"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, comboSources4);


        formLayout_3->setWidget(1, QFormLayout::SpanningRole, widget);

        checkSourcesDefault = new QCheckBox(tab);
        checkSourcesDefault->setObjectName(QString::fromUtf8("checkSourcesDefault"));
        checkSourcesDefault->setChecked(true);

        formLayout_3->setWidget(0, QFormLayout::SpanningRole, checkSourcesDefault);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        formLayout_4 = new QFormLayout(tab_2);
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        label = new QLabel(tab_2);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label);

        lineAuthPseudo = new QLineEdit(tab_2);
        lineAuthPseudo->setObjectName(QString::fromUtf8("lineAuthPseudo"));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, lineAuthPseudo);

        label_2 = new QLabel(tab_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineAuthPassword = new QLineEdit(tab_2);
        lineAuthPassword->setObjectName(QString::fromUtf8("lineAuthPassword"));

        horizontalLayout->addWidget(lineAuthPassword);

        buttonAuthHash = new QPushButton(tab_2);
        buttonAuthHash->setObjectName(QString::fromUtf8("buttonAuthHash"));

        horizontalLayout->addWidget(buttonAuthHash);


        formLayout_4->setLayout(1, QFormLayout::FieldRole, horizontalLayout);

        tabWidget->addTab(tab_2, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        formLayout_7 = new QFormLayout(tab_4);
        formLayout_7->setObjectName(QString::fromUtf8("formLayout_7"));
        checkLoginParameter = new QCheckBox(tab_4);
        checkLoginParameter->setObjectName(QString::fromUtf8("checkLoginParameter"));
        checkLoginParameter->setChecked(true);

        formLayout_7->setWidget(0, QFormLayout::SpanningRole, checkLoginParameter);

        widget_2 = new QWidget(tab_4);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setEnabled(false);
        formLayout_6 = new QFormLayout(widget_2);
        formLayout_6->setObjectName(QString::fromUtf8("formLayout_6"));
        formLayout_6->setContentsMargins(18, 0, 0, 0);
        label_8 = new QLabel(widget_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, label_8);

        comboLoginMethod = new QComboBox(widget_2);
        comboLoginMethod->setObjectName(QString::fromUtf8("comboLoginMethod"));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, comboLoginMethod);

        label_9 = new QLabel(widget_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_6->setWidget(2, QFormLayout::LabelRole, label_9);

        label_10 = new QLabel(widget_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_6->setWidget(3, QFormLayout::LabelRole, label_10);

        label_11 = new QLabel(widget_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_6->setWidget(1, QFormLayout::LabelRole, label_11);

        lineLoginUrl = new QLineEdit(widget_2);
        lineLoginUrl->setObjectName(QString::fromUtf8("lineLoginUrl"));

        formLayout_6->setWidget(1, QFormLayout::FieldRole, lineLoginUrl);

        lineLoginPseudo = new QLineEdit(widget_2);
        lineLoginPseudo->setObjectName(QString::fromUtf8("lineLoginPseudo"));

        formLayout_6->setWidget(2, QFormLayout::FieldRole, lineLoginPseudo);

        lineLoginPassword = new QLineEdit(widget_2);
        lineLoginPassword->setObjectName(QString::fromUtf8("lineLoginPassword"));

        formLayout_6->setWidget(3, QFormLayout::FieldRole, lineLoginPassword);


        formLayout_7->setWidget(1, QFormLayout::SpanningRole, widget_2);

        tabWidget->addTab(tab_4, QString());

        formLayout->setWidget(0, QFormLayout::LabelRole, tabWidget);


        retranslateUi(SourcesSettingsWindow);
        QObject::connect(checkSourcesDefault, SIGNAL(toggled(bool)), widget, SLOT(setDisabled(bool)));
        QObject::connect(pushButton, SIGNAL(clicked()), SourcesSettingsWindow, SLOT(deleteSite()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), SourcesSettingsWindow, SLOT(reject()));
        QObject::connect(pushButton_3, SIGNAL(clicked()), SourcesSettingsWindow, SLOT(accept()));
        QObject::connect(checkLoginParameter, SIGNAL(toggled(bool)), widget_2, SLOT(setDisabled(bool)));

        tabWidget->setCurrentIndex(0);
        comboSources1->setCurrentIndex(0);
        comboSources2->setCurrentIndex(1);
        comboSources3->setCurrentIndex(2);
        comboSources4->setCurrentIndex(3);
        comboLoginMethod->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(SourcesSettingsWindow);
    } // setupUi

    void retranslateUi(QDialog *SourcesSettingsWindow)
    {
        SourcesSettingsWindow->setWindowTitle(QApplication::translate("SourcesSettingsWindow", "Options de site", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("SourcesSettingsWindow", "Supprimer", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("SourcesSettingsWindow", "Annuler", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("SourcesSettingsWindow", "Valider", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("SourcesSettingsWindow", "Referer (par d\303\251faut)", 0, QApplication::UnicodeUTF8));
        comboReferer->clear();
        comboReferer->insertItems(0, QStringList()
         << QApplication::translate("SourcesSettingsWindow", "Aucun", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Site", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Page", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Image", 0, QApplication::UnicodeUTF8)
        );
        label_12->setText(QApplication::translate("SourcesSettingsWindow", "Referer (apercu)", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("SourcesSettingsWindow", "Referer (image)", 0, QApplication::UnicodeUTF8));
        comboRefererPreview->clear();
        comboRefererPreview->insertItems(0, QStringList()
         << QApplication::translate("SourcesSettingsWindow", "Par d\303\251faut", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Aucun", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Site", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Page", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Image", 0, QApplication::UnicodeUTF8)
        );
        comboRefererImage->clear();
        comboRefererImage->insertItems(0, QStringList()
         << QApplication::translate("SourcesSettingsWindow", "Par d\303\251faut", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Aucun", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Site", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Page", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "D\303\251tails", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Image", 0, QApplication::UnicodeUTF8)
        );
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("SourcesSettingsWindow", "G\303\251n\303\251ral", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("SourcesSettingsWindow", "Source 1", 0, QApplication::UnicodeUTF8));
        comboSources1->clear();
        comboSources1->insertItems(0, QStringList()
         << QApplication::translate("SourcesSettingsWindow", "XML", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "JSON", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Regex", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "RSS", 0, QApplication::UnicodeUTF8)
        );
        label_4->setText(QApplication::translate("SourcesSettingsWindow", "Source 2", 0, QApplication::UnicodeUTF8));
        comboSources2->clear();
        comboSources2->insertItems(0, QStringList()
         << QApplication::translate("SourcesSettingsWindow", "XML", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "JSON", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Regex", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "RSS", 0, QApplication::UnicodeUTF8)
        );
        label_5->setText(QApplication::translate("SourcesSettingsWindow", "Source 3", 0, QApplication::UnicodeUTF8));
        comboSources3->clear();
        comboSources3->insertItems(0, QStringList()
         << QApplication::translate("SourcesSettingsWindow", "XML", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "JSON", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Regex", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "RSS", 0, QApplication::UnicodeUTF8)
        );
        label_6->setText(QApplication::translate("SourcesSettingsWindow", "Source 4", 0, QApplication::UnicodeUTF8));
        comboSources4->clear();
        comboSources4->insertItems(0, QStringList()
         << QApplication::translate("SourcesSettingsWindow", "XML", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "JSON", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "Regex", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "RSS", 0, QApplication::UnicodeUTF8)
        );
        checkSourcesDefault->setText(QApplication::translate("SourcesSettingsWindow", "Utiliser les sources par d\303\251faut", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("SourcesSettingsWindow", "Sources", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SourcesSettingsWindow", "Pseudo", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SourcesSettingsWindow", "Mot de passe hash\303\251", 0, QApplication::UnicodeUTF8));
        buttonAuthHash->setText(QApplication::translate("SourcesSettingsWindow", "Hasher un mot de passe", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("SourcesSettingsWindow", "Identifiants", 0, QApplication::UnicodeUTF8));
        checkLoginParameter->setText(QApplication::translate("SourcesSettingsWindow", "Passage dans l'url", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("SourcesSettingsWindow", "M\303\251thode", 0, QApplication::UnicodeUTF8));
        comboLoginMethod->clear();
        comboLoginMethod->insertItems(0, QStringList()
         << QApplication::translate("SourcesSettingsWindow", "GET", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SourcesSettingsWindow", "POST", 0, QApplication::UnicodeUTF8)
        );
        label_9->setText(QApplication::translate("SourcesSettingsWindow", "Pseudo", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("SourcesSettingsWindow", "Password", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("SourcesSettingsWindow", "URL", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("SourcesSettingsWindow", "Connexion", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SourcesSettingsWindow: public Ui_SourcesSettingsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SOURCESSETTINGSWINDOW_H
