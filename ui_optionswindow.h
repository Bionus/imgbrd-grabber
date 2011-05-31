/********************************************************************************
** Form generated from reading UI file 'optionswindow.ui'
**
** Created: Tue 31. May 15:53:35 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONSWINDOW_H
#define UI_OPTIONSWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_optionsWindow
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeWidget;
    QVBoxLayout *container;
    QWidget *widget_2;
    QFormLayout *formLayout_2;
    QLabel *label;
    QComboBox *comboLanguages;
    QLabel *label_2;
    QSpinBox *spinCheckForUpdates;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *lineDateFormat;
    QLabel *label_5;
    QLineEdit *lineBlacklist;
    QLabel *label_6;
    QCheckBox *checkDownloadBlacklisted;
    QCheckBox *checkLoadFirstAtStart;
    QWidget *widget_1;
    QFormLayout *formLayout_3;
    QLabel *label_9;
    QSpinBox *spinImagesPerPage;
    QLabel *label_10;
    QSpinBox *spinColumns;
    QLabel *label_11;
    QComboBox *comboSource1;
    QLabel *label_12;
    QComboBox *comboSource2;
    QLabel *label_13;
    QComboBox *comboSource3;
    QWidget *widget;
    QFormLayout *formLayout;
    QCheckBox *checkShowLog;
    QCheckBox *checkInvertLog;
    QWidget *widget_3;
    QFormLayout *formLayout_4;
    QCheckBox *checkDownloadOriginals;
    QLabel *label_7;
    QLabel *label_14;
    QLabel *label_15;
    QLineEdit *lineSeparator;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineFolder;
    QPushButton *buttonFolder;
    QLabel *label_8;
    QLineEdit *lineFilename;
    QLabel *filenameValidator;
    QWidget *widget_4;
    QFormLayout *formLayout_5;
    QLabel *label_16;
    QLabel *label_18;
    QLabel *label_19;
    QCheckBox *checkArtistsKeepAll;
    QLineEdit *lineArtistsIfNone;
    QLineEdit *lineArtistsSeparator;
    QLineEdit *lineArtistsIfMultiples;
    QWidget *widget_5;
    QFormLayout *formLayout_6;
    QLabel *label_17;
    QLabel *label_20;
    QLabel *label_21;
    QLineEdit *lineCopyrightsIfNone;
    QLineEdit *lineCopyrightsSeparator;
    QLineEdit *lineCopyrightsIfMultiples;
    QCheckBox *checkCopyrightsUseShorter;
    QCheckBox *checkCopyrightsKeepAll;
    QWidget *widget_6;
    QFormLayout *formLayout_7;
    QLabel *label_22;
    QLabel *label_23;
    QLabel *label_24;
    QLineEdit *lineCharactersIfNone;
    QLineEdit *lineCharactersSeparator;
    QLineEdit *lineCharactersIfMultiples;
    QCheckBox *checkCharactersKeepAll;
    QWidget *widget_8;
    QFormLayout *formLayout_9;
    QLabel *label_30;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *lineColoringArtists;
    QPushButton *buttonColoringArtists;
    QLabel *label_31;
    QLabel *label_32;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *lineColoringCopyrights;
    QPushButton *buttonColoringCopyrights;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *lineColoringCharacters;
    QPushButton *buttonColoringCharacters;
    QLabel *label_33;
    QLabel *label_34;
    QHBoxLayout *horizontalLayout_6;
    QLineEdit *lineColoringGenerals;
    QPushButton *buttonColoringGenerals;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *lineColoringModels;
    QPushButton *buttonColoringModels;
    QWidget *widget_7;
    QFormLayout *formLayout_8;
    QLabel *label_25;
    QLabel *label_26;
    QLabel *label_27;
    QLabel *label_28;
    QLabel *label_29;
    QLineEdit *lineCommandsInitialisation;
    QLineEdit *lineCommandsImage;
    QLineEdit *lineCommandsTag;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *optionsWindow)
    {
        if (optionsWindow->objectName().isEmpty())
            optionsWindow->setObjectName(QString::fromUtf8("optionsWindow"));
        optionsWindow->resize(646, 1560);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        optionsWindow->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(optionsWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        treeWidget = new QTreeWidget(optionsWindow);
        new QTreeWidgetItem(treeWidget);
        new QTreeWidgetItem(treeWidget);
        new QTreeWidgetItem(treeWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem(treeWidget);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(treeWidget);
        new QTreeWidgetItem(treeWidget);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
        treeWidget->setSizePolicy(sizePolicy);
        treeWidget->setMaximumSize(QSize(160, 16777215));
        treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeWidget->setAutoScroll(true);
        treeWidget->setItemsExpandable(true);
        treeWidget->setWordWrap(true);
        treeWidget->header()->setVisible(false);

        horizontalLayout->addWidget(treeWidget);

        container = new QVBoxLayout();
        container->setObjectName(QString::fromUtf8("container"));
        widget_2 = new QWidget(optionsWindow);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        formLayout_2 = new QFormLayout(widget_2);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label = new QLabel(widget_2);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        comboLanguages = new QComboBox(widget_2);
        comboLanguages->setObjectName(QString::fromUtf8("comboLanguages"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboLanguages);

        label_2 = new QLabel(widget_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_2);

        spinCheckForUpdates = new QSpinBox(widget_2);
        spinCheckForUpdates->setObjectName(QString::fromUtf8("spinCheckForUpdates"));
        spinCheckForUpdates->setMinimum(-1);
        spinCheckForUpdates->setMaximum(999999999);
        spinCheckForUpdates->setValue(86400);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, spinCheckForUpdates);

        label_3 = new QLabel(widget_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setWordWrap(true);

        formLayout_2->setWidget(3, QFormLayout::SpanningRole, label_3);

        label_4 = new QLabel(widget_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_4);

        lineDateFormat = new QLineEdit(widget_2);
        lineDateFormat->setObjectName(QString::fromUtf8("lineDateFormat"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, lineDateFormat);

        label_5 = new QLabel(widget_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_5);

        lineBlacklist = new QLineEdit(widget_2);
        lineBlacklist->setObjectName(QString::fromUtf8("lineBlacklist"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, lineBlacklist);

        label_6 = new QLabel(widget_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setWordWrap(true);

        formLayout_2->setWidget(6, QFormLayout::SpanningRole, label_6);

        checkDownloadBlacklisted = new QCheckBox(widget_2);
        checkDownloadBlacklisted->setObjectName(QString::fromUtf8("checkDownloadBlacklisted"));

        formLayout_2->setWidget(7, QFormLayout::SpanningRole, checkDownloadBlacklisted);

        checkLoadFirstAtStart = new QCheckBox(widget_2);
        checkLoadFirstAtStart->setObjectName(QString::fromUtf8("checkLoadFirstAtStart"));

        formLayout_2->setWidget(8, QFormLayout::SpanningRole, checkLoadFirstAtStart);


        container->addWidget(widget_2);

        widget_1 = new QWidget(optionsWindow);
        widget_1->setObjectName(QString::fromUtf8("widget_1"));
        formLayout_3 = new QFormLayout(widget_1);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_9 = new QLabel(widget_1);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_9);

        spinImagesPerPage = new QSpinBox(widget_1);
        spinImagesPerPage->setObjectName(QString::fromUtf8("spinImagesPerPage"));
        spinImagesPerPage->setMinimum(1);
        spinImagesPerPage->setMaximum(1000);
        spinImagesPerPage->setValue(20);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, spinImagesPerPage);

        label_10 = new QLabel(widget_1);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_10);

        spinColumns = new QSpinBox(widget_1);
        spinColumns->setObjectName(QString::fromUtf8("spinColumns"));
        spinColumns->setMinimum(1);
        spinColumns->setMaximum(10);
        spinColumns->setValue(1);

        formLayout_3->setWidget(1, QFormLayout::FieldRole, spinColumns);

        label_11 = new QLabel(widget_1);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_11);

        comboSource1 = new QComboBox(widget_1);
        comboSource1->setObjectName(QString::fromUtf8("comboSource1"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, comboSource1);

        label_12 = new QLabel(widget_1);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_3->setWidget(3, QFormLayout::LabelRole, label_12);

        comboSource2 = new QComboBox(widget_1);
        comboSource2->setObjectName(QString::fromUtf8("comboSource2"));

        formLayout_3->setWidget(3, QFormLayout::FieldRole, comboSource2);

        label_13 = new QLabel(widget_1);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        formLayout_3->setWidget(4, QFormLayout::LabelRole, label_13);

        comboSource3 = new QComboBox(widget_1);
        comboSource3->setObjectName(QString::fromUtf8("comboSource3"));

        formLayout_3->setWidget(4, QFormLayout::FieldRole, comboSource3);


        container->addWidget(widget_1);

        widget = new QWidget(optionsWindow);
        widget->setObjectName(QString::fromUtf8("widget"));
        formLayout = new QFormLayout(widget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        checkShowLog = new QCheckBox(widget);
        checkShowLog->setObjectName(QString::fromUtf8("checkShowLog"));
        checkShowLog->setChecked(true);

        formLayout->setWidget(0, QFormLayout::SpanningRole, checkShowLog);

        checkInvertLog = new QCheckBox(widget);
        checkInvertLog->setObjectName(QString::fromUtf8("checkInvertLog"));

        formLayout->setWidget(1, QFormLayout::SpanningRole, checkInvertLog);


        container->addWidget(widget);

        widget_3 = new QWidget(optionsWindow);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        formLayout_4 = new QFormLayout(widget_3);
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        checkDownloadOriginals = new QCheckBox(widget_3);
        checkDownloadOriginals->setObjectName(QString::fromUtf8("checkDownloadOriginals"));
        checkDownloadOriginals->setChecked(true);

        formLayout_4->setWidget(0, QFormLayout::SpanningRole, checkDownloadOriginals);

        label_7 = new QLabel(widget_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, label_7);

        label_14 = new QLabel(widget_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        formLayout_4->setWidget(3, QFormLayout::LabelRole, label_14);

        label_15 = new QLabel(widget_3);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setWordWrap(true);

        formLayout_4->setWidget(6, QFormLayout::SpanningRole, label_15);

        lineSeparator = new QLineEdit(widget_3);
        lineSeparator->setObjectName(QString::fromUtf8("lineSeparator"));

        formLayout_4->setWidget(3, QFormLayout::FieldRole, lineSeparator);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lineFolder = new QLineEdit(widget_3);
        lineFolder->setObjectName(QString::fromUtf8("lineFolder"));

        horizontalLayout_2->addWidget(lineFolder);

        buttonFolder = new QPushButton(widget_3);
        buttonFolder->setObjectName(QString::fromUtf8("buttonFolder"));

        horizontalLayout_2->addWidget(buttonFolder);

        horizontalLayout_2->setStretch(0, 1);

        formLayout_4->setLayout(1, QFormLayout::FieldRole, horizontalLayout_2);

        label_8 = new QLabel(widget_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_4->setWidget(4, QFormLayout::LabelRole, label_8);

        lineFilename = new QLineEdit(widget_3);
        lineFilename->setObjectName(QString::fromUtf8("lineFilename"));

        formLayout_4->setWidget(4, QFormLayout::FieldRole, lineFilename);

        filenameValidator = new QLabel(widget_3);
        filenameValidator->setObjectName(QString::fromUtf8("filenameValidator"));
        filenameValidator->setWordWrap(true);

        formLayout_4->setWidget(5, QFormLayout::SpanningRole, filenameValidator);


        container->addWidget(widget_3);

        widget_4 = new QWidget(optionsWindow);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        formLayout_5 = new QFormLayout(widget_4);
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        formLayout_5->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_16 = new QLabel(widget_4);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, label_16);

        label_18 = new QLabel(widget_4);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        formLayout_5->setWidget(2, QFormLayout::LabelRole, label_18);

        label_19 = new QLabel(widget_4);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        formLayout_5->setWidget(3, QFormLayout::LabelRole, label_19);

        checkArtistsKeepAll = new QCheckBox(widget_4);
        checkArtistsKeepAll->setObjectName(QString::fromUtf8("checkArtistsKeepAll"));

        formLayout_5->setWidget(1, QFormLayout::SpanningRole, checkArtistsKeepAll);

        lineArtistsIfNone = new QLineEdit(widget_4);
        lineArtistsIfNone->setObjectName(QString::fromUtf8("lineArtistsIfNone"));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, lineArtistsIfNone);

        lineArtistsSeparator = new QLineEdit(widget_4);
        lineArtistsSeparator->setObjectName(QString::fromUtf8("lineArtistsSeparator"));

        formLayout_5->setWidget(2, QFormLayout::FieldRole, lineArtistsSeparator);

        lineArtistsIfMultiples = new QLineEdit(widget_4);
        lineArtistsIfMultiples->setObjectName(QString::fromUtf8("lineArtistsIfMultiples"));

        formLayout_5->setWidget(3, QFormLayout::FieldRole, lineArtistsIfMultiples);


        container->addWidget(widget_4);

        widget_5 = new QWidget(optionsWindow);
        widget_5->setObjectName(QString::fromUtf8("widget_5"));
        formLayout_6 = new QFormLayout(widget_5);
        formLayout_6->setObjectName(QString::fromUtf8("formLayout_6"));
        label_17 = new QLabel(widget_5);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, label_17);

        label_20 = new QLabel(widget_5);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        formLayout_6->setWidget(3, QFormLayout::LabelRole, label_20);

        label_21 = new QLabel(widget_5);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        formLayout_6->setWidget(4, QFormLayout::LabelRole, label_21);

        lineCopyrightsIfNone = new QLineEdit(widget_5);
        lineCopyrightsIfNone->setObjectName(QString::fromUtf8("lineCopyrightsIfNone"));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, lineCopyrightsIfNone);

        lineCopyrightsSeparator = new QLineEdit(widget_5);
        lineCopyrightsSeparator->setObjectName(QString::fromUtf8("lineCopyrightsSeparator"));

        formLayout_6->setWidget(3, QFormLayout::FieldRole, lineCopyrightsSeparator);

        lineCopyrightsIfMultiples = new QLineEdit(widget_5);
        lineCopyrightsIfMultiples->setObjectName(QString::fromUtf8("lineCopyrightsIfMultiples"));

        formLayout_6->setWidget(4, QFormLayout::FieldRole, lineCopyrightsIfMultiples);

        checkCopyrightsUseShorter = new QCheckBox(widget_5);
        checkCopyrightsUseShorter->setObjectName(QString::fromUtf8("checkCopyrightsUseShorter"));
        checkCopyrightsUseShorter->setChecked(true);

        formLayout_6->setWidget(1, QFormLayout::SpanningRole, checkCopyrightsUseShorter);

        checkCopyrightsKeepAll = new QCheckBox(widget_5);
        checkCopyrightsKeepAll->setObjectName(QString::fromUtf8("checkCopyrightsKeepAll"));

        formLayout_6->setWidget(2, QFormLayout::SpanningRole, checkCopyrightsKeepAll);


        container->addWidget(widget_5);

        widget_6 = new QWidget(optionsWindow);
        widget_6->setObjectName(QString::fromUtf8("widget_6"));
        formLayout_7 = new QFormLayout(widget_6);
        formLayout_7->setObjectName(QString::fromUtf8("formLayout_7"));
        label_22 = new QLabel(widget_6);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        formLayout_7->setWidget(0, QFormLayout::LabelRole, label_22);

        label_23 = new QLabel(widget_6);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        formLayout_7->setWidget(2, QFormLayout::LabelRole, label_23);

        label_24 = new QLabel(widget_6);
        label_24->setObjectName(QString::fromUtf8("label_24"));

        formLayout_7->setWidget(3, QFormLayout::LabelRole, label_24);

        lineCharactersIfNone = new QLineEdit(widget_6);
        lineCharactersIfNone->setObjectName(QString::fromUtf8("lineCharactersIfNone"));

        formLayout_7->setWidget(0, QFormLayout::FieldRole, lineCharactersIfNone);

        lineCharactersSeparator = new QLineEdit(widget_6);
        lineCharactersSeparator->setObjectName(QString::fromUtf8("lineCharactersSeparator"));

        formLayout_7->setWidget(2, QFormLayout::FieldRole, lineCharactersSeparator);

        lineCharactersIfMultiples = new QLineEdit(widget_6);
        lineCharactersIfMultiples->setObjectName(QString::fromUtf8("lineCharactersIfMultiples"));

        formLayout_7->setWidget(3, QFormLayout::FieldRole, lineCharactersIfMultiples);

        checkCharactersKeepAll = new QCheckBox(widget_6);
        checkCharactersKeepAll->setObjectName(QString::fromUtf8("checkCharactersKeepAll"));

        formLayout_7->setWidget(1, QFormLayout::SpanningRole, checkCharactersKeepAll);


        container->addWidget(widget_6);

        widget_8 = new QWidget(optionsWindow);
        widget_8->setObjectName(QString::fromUtf8("widget_8"));
        formLayout_9 = new QFormLayout(widget_8);
        formLayout_9->setObjectName(QString::fromUtf8("formLayout_9"));
        label_30 = new QLabel(widget_8);
        label_30->setObjectName(QString::fromUtf8("label_30"));

        formLayout_9->setWidget(0, QFormLayout::LabelRole, label_30);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        lineColoringArtists = new QLineEdit(widget_8);
        lineColoringArtists->setObjectName(QString::fromUtf8("lineColoringArtists"));

        horizontalLayout_3->addWidget(lineColoringArtists);

        buttonColoringArtists = new QPushButton(widget_8);
        buttonColoringArtists->setObjectName(QString::fromUtf8("buttonColoringArtists"));

        horizontalLayout_3->addWidget(buttonColoringArtists);

        horizontalLayout_3->setStretch(0, 1);

        formLayout_9->setLayout(0, QFormLayout::FieldRole, horizontalLayout_3);

        label_31 = new QLabel(widget_8);
        label_31->setObjectName(QString::fromUtf8("label_31"));

        formLayout_9->setWidget(1, QFormLayout::LabelRole, label_31);

        label_32 = new QLabel(widget_8);
        label_32->setObjectName(QString::fromUtf8("label_32"));

        formLayout_9->setWidget(2, QFormLayout::LabelRole, label_32);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        lineColoringCopyrights = new QLineEdit(widget_8);
        lineColoringCopyrights->setObjectName(QString::fromUtf8("lineColoringCopyrights"));

        horizontalLayout_4->addWidget(lineColoringCopyrights);

        buttonColoringCopyrights = new QPushButton(widget_8);
        buttonColoringCopyrights->setObjectName(QString::fromUtf8("buttonColoringCopyrights"));

        horizontalLayout_4->addWidget(buttonColoringCopyrights);


        formLayout_9->setLayout(1, QFormLayout::FieldRole, horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        lineColoringCharacters = new QLineEdit(widget_8);
        lineColoringCharacters->setObjectName(QString::fromUtf8("lineColoringCharacters"));

        horizontalLayout_5->addWidget(lineColoringCharacters);

        buttonColoringCharacters = new QPushButton(widget_8);
        buttonColoringCharacters->setObjectName(QString::fromUtf8("buttonColoringCharacters"));

        horizontalLayout_5->addWidget(buttonColoringCharacters);


        formLayout_9->setLayout(2, QFormLayout::FieldRole, horizontalLayout_5);

        label_33 = new QLabel(widget_8);
        label_33->setObjectName(QString::fromUtf8("label_33"));

        formLayout_9->setWidget(3, QFormLayout::LabelRole, label_33);

        label_34 = new QLabel(widget_8);
        label_34->setObjectName(QString::fromUtf8("label_34"));

        formLayout_9->setWidget(4, QFormLayout::LabelRole, label_34);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        lineColoringGenerals = new QLineEdit(widget_8);
        lineColoringGenerals->setObjectName(QString::fromUtf8("lineColoringGenerals"));

        horizontalLayout_6->addWidget(lineColoringGenerals);

        buttonColoringGenerals = new QPushButton(widget_8);
        buttonColoringGenerals->setObjectName(QString::fromUtf8("buttonColoringGenerals"));

        horizontalLayout_6->addWidget(buttonColoringGenerals);

        horizontalLayout_6->setStretch(0, 1);

        formLayout_9->setLayout(4, QFormLayout::FieldRole, horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        lineColoringModels = new QLineEdit(widget_8);
        lineColoringModels->setObjectName(QString::fromUtf8("lineColoringModels"));

        horizontalLayout_7->addWidget(lineColoringModels);

        buttonColoringModels = new QPushButton(widget_8);
        buttonColoringModels->setObjectName(QString::fromUtf8("buttonColoringModels"));

        horizontalLayout_7->addWidget(buttonColoringModels);

        horizontalLayout_7->setStretch(0, 1);

        formLayout_9->setLayout(3, QFormLayout::FieldRole, horizontalLayout_7);


        container->addWidget(widget_8);

        widget_7 = new QWidget(optionsWindow);
        widget_7->setObjectName(QString::fromUtf8("widget_7"));
        formLayout_8 = new QFormLayout(widget_7);
        formLayout_8->setObjectName(QString::fromUtf8("formLayout_8"));
        label_25 = new QLabel(widget_7);
        label_25->setObjectName(QString::fromUtf8("label_25"));

        formLayout_8->setWidget(0, QFormLayout::LabelRole, label_25);

        label_26 = new QLabel(widget_7);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        formLayout_8->setWidget(1, QFormLayout::LabelRole, label_26);

        label_27 = new QLabel(widget_7);
        label_27->setObjectName(QString::fromUtf8("label_27"));

        formLayout_8->setWidget(2, QFormLayout::SpanningRole, label_27);

        label_28 = new QLabel(widget_7);
        label_28->setObjectName(QString::fromUtf8("label_28"));

        formLayout_8->setWidget(3, QFormLayout::LabelRole, label_28);

        label_29 = new QLabel(widget_7);
        label_29->setObjectName(QString::fromUtf8("label_29"));

        formLayout_8->setWidget(4, QFormLayout::SpanningRole, label_29);

        lineCommandsInitialisation = new QLineEdit(widget_7);
        lineCommandsInitialisation->setObjectName(QString::fromUtf8("lineCommandsInitialisation"));

        formLayout_8->setWidget(0, QFormLayout::FieldRole, lineCommandsInitialisation);

        lineCommandsImage = new QLineEdit(widget_7);
        lineCommandsImage->setObjectName(QString::fromUtf8("lineCommandsImage"));

        formLayout_8->setWidget(1, QFormLayout::FieldRole, lineCommandsImage);

        lineCommandsTag = new QLineEdit(widget_7);
        lineCommandsTag->setObjectName(QString::fromUtf8("lineCommandsTag"));

        formLayout_8->setWidget(3, QFormLayout::FieldRole, lineCommandsTag);


        container->addWidget(widget_7);


        horizontalLayout->addLayout(container);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(optionsWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(optionsWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), optionsWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), optionsWindow, SLOT(reject()));
        QObject::connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), optionsWindow, SLOT(updateContainer(QTreeWidgetItem*,QTreeWidgetItem*)));

        comboSource2->setCurrentIndex(1);
        comboSource3->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(optionsWindow);
    } // setupUi

    void retranslateUi(QDialog *optionsWindow)
    {
        optionsWindow->setWindowTitle(QApplication::translate("optionsWindow", "Grabber - Options", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("optionsWindow", "1", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = treeWidget->isSortingEnabled();
        treeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("optionsWindow", "G\303\251n\303\251ral", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = treeWidget->topLevelItem(1);
        ___qtreewidgetitem2->setText(0, QApplication::translate("optionsWindow", "Sources", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = treeWidget->topLevelItem(2);
        ___qtreewidgetitem3->setText(0, QApplication::translate("optionsWindow", "Log", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem4 = treeWidget->topLevelItem(3);
        ___qtreewidgetitem4->setText(0, QApplication::translate("optionsWindow", "Sauvegarde", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem5 = ___qtreewidgetitem4->child(0);
        ___qtreewidgetitem5->setText(0, QApplication::translate("optionsWindow", "Tags artiste", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem6 = ___qtreewidgetitem4->child(1);
        ___qtreewidgetitem6->setText(0, QApplication::translate("optionsWindow", "Tags s\303\251rie", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem7 = ___qtreewidgetitem4->child(2);
        ___qtreewidgetitem7->setText(0, QApplication::translate("optionsWindow", "Tags personnage", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem8 = treeWidget->topLevelItem(4);
        ___qtreewidgetitem8->setText(0, QApplication::translate("optionsWindow", "Coloration", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem9 = treeWidget->topLevelItem(5);
        ___qtreewidgetitem9->setText(0, QApplication::translate("optionsWindow", "Commandes", 0, QApplication::UnicodeUTF8));
        treeWidget->setSortingEnabled(__sortingEnabled);

        label->setText(QApplication::translate("optionsWindow", "Langue", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("optionsWindow", "Recherche de mises \303\240 jour", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("optionsWindow", "<i>En secondes. Mettez -1 pour ne jamais rechercher de mises \303\240 jour. Vous pourrez toujours lancer le gestionnaire de mises \303\240 jour manuellement en lan\303\247ant \"Updater.exe\".</i>", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("optionsWindow", "Format de la date", 0, QApplication::UnicodeUTF8));
        lineDateFormat->setText(QString());
        label_5->setText(QApplication::translate("optionsWindow", "Liste noire", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("optionsWindow", "<i>Une confirmation sera demand\303\251e avant l'affichage d'une image contenant un tag contenu dans cette liste (s\303\251parer les tags par des espaces).</i>", 0, QApplication::UnicodeUTF8));
        checkDownloadBlacklisted->setText(QApplication::translate("optionsWindow", "T\303\251l\303\251charger les images de la liste noire", 0, QApplication::UnicodeUTF8));
        checkLoadFirstAtStart->setText(QApplication::translate("optionsWindow", "Charger la premi\303\250re page au d\303\251marrage", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("optionsWindow", "Images par page", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("optionsWindow", "Nombre de colonnes", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("optionsWindow", "Source 1", 0, QApplication::UnicodeUTF8));
        comboSource1->clear();
        comboSource1->insertItems(0, QStringList()
         << QApplication::translate("optionsWindow", "XML", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("optionsWindow", "JSON", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("optionsWindow", "Regex", 0, QApplication::UnicodeUTF8)
        );
        label_12->setText(QApplication::translate("optionsWindow", "Source 2", 0, QApplication::UnicodeUTF8));
        comboSource2->clear();
        comboSource2->insertItems(0, QStringList()
         << QApplication::translate("optionsWindow", "XML", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("optionsWindow", "JSON", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("optionsWindow", "Regex", 0, QApplication::UnicodeUTF8)
        );
        label_13->setText(QApplication::translate("optionsWindow", "Source 3", 0, QApplication::UnicodeUTF8));
        comboSource3->clear();
        comboSource3->insertItems(0, QStringList()
         << QApplication::translate("optionsWindow", "XML", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("optionsWindow", "JSON", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("optionsWindow", "Regex", 0, QApplication::UnicodeUTF8)
        );
        checkShowLog->setText(QApplication::translate("optionsWindow", "Afficher le log", 0, QApplication::UnicodeUTF8));
        checkInvertLog->setText(QApplication::translate("optionsWindow", "Inverser le log", 0, QApplication::UnicodeUTF8));
        checkDownloadOriginals->setText(QApplication::translate("optionsWindow", "T\303\251l\303\251charger les images originales", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("optionsWindow", "Dossier", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("optionsWindow", "S\303\251parateur de tags", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("optionsWindow", "Symboles disponibles : <i>%artist%</i>, <i>%general%</i>, <i>%copyright%</i>, <i>%character%</i>, <i>%all%</i>, <i>%filename%</i>, <i>%ext%</i>, <i>%rating%</i>, <i>%website%</i>, <i>%md5%</i>.<br/><i>%artist%</i> : tags de nom d'artiste<br/><i>%general%</i> : tags d'image g\303\251n\303\251riques<br/><i>%copyright%</i> : tags de copyright, en g\303\251n\303\251ral le nom de l'anime/manga<br/><i>%character%</i> : tags indiquant quels personnages sont pr\303\251sents sur l'image<br/><i>%all%</i> : tous les tags<br/><i>%filename%</i> : nom du fichier sur le serveur, en g\303\251n\303\251ral une cha\303\256ne alphanum\303\251rique<br/><i>%ext%</i> : extension de l'image<br/><i>%rating%</i> : Questionable, Safe ou Explicit<br/><i>%website%</i> : url du site de l'image<br/><i>%md5%</i> : code unique de l'image, compos\303\251 de 32 caract\303\250res alphanum\303\251riques", 0, QApplication::UnicodeUTF8));
        lineSeparator->setText(QString());
        buttonFolder->setText(QApplication::translate("optionsWindow", "Parcourir", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("optionsWindow", "Noms de fichiers", 0, QApplication::UnicodeUTF8));
        filenameValidator->setText(QString());
        label_16->setText(QApplication::translate("optionsWindow", "Si aucun", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("optionsWindow", "S\303\251parateur si multiples", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("optionsWindow", "Valeur si multiples", 0, QApplication::UnicodeUTF8));
        checkArtistsKeepAll->setText(QApplication::translate("optionsWindow", "Garder tous", 0, QApplication::UnicodeUTF8));
        lineArtistsIfNone->setText(QApplication::translate("optionsWindow", "anonymous", 0, QApplication::UnicodeUTF8));
        lineArtistsSeparator->setText(QApplication::translate("optionsWindow", "+", 0, QApplication::UnicodeUTF8));
        lineArtistsIfMultiples->setText(QApplication::translate("optionsWindow", "multiple artists", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("optionsWindow", "Si aucun", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("optionsWindow", "S\303\251parateur si multiples", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("optionsWindow", "Valeur si multiples", 0, QApplication::UnicodeUTF8));
        lineCopyrightsIfNone->setText(QApplication::translate("optionsWindow", "misc", 0, QApplication::UnicodeUTF8));
        lineCopyrightsSeparator->setText(QApplication::translate("optionsWindow", "+", 0, QApplication::UnicodeUTF8));
        lineCopyrightsIfMultiples->setText(QApplication::translate("optionsWindow", "crossover", 0, QApplication::UnicodeUTF8));
        checkCopyrightsUseShorter->setText(QApplication::translate("optionsWindow", "Utiliser le plus court si possible", 0, QApplication::UnicodeUTF8));
        checkCopyrightsKeepAll->setText(QApplication::translate("optionsWindow", "Garder tous", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("optionsWindow", "Si aucun", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("optionsWindow", "S\303\251parateur si multiples", 0, QApplication::UnicodeUTF8));
        label_24->setText(QApplication::translate("optionsWindow", "Valeur si multiples", 0, QApplication::UnicodeUTF8));
        lineCharactersIfNone->setText(QApplication::translate("optionsWindow", "unknown", 0, QApplication::UnicodeUTF8));
        lineCharactersSeparator->setText(QApplication::translate("optionsWindow", "+", 0, QApplication::UnicodeUTF8));
        lineCharactersIfMultiples->setText(QApplication::translate("optionsWindow", "group", 0, QApplication::UnicodeUTF8));
        checkCharactersKeepAll->setText(QApplication::translate("optionsWindow", "Garder tous", 0, QApplication::UnicodeUTF8));
        label_30->setText(QApplication::translate("optionsWindow", "Artistes", 0, QApplication::UnicodeUTF8));
        lineColoringArtists->setText(QString());
        buttonColoringArtists->setText(QApplication::translate("optionsWindow", "Choisir", 0, QApplication::UnicodeUTF8));
        label_31->setText(QApplication::translate("optionsWindow", "S\303\251ries", 0, QApplication::UnicodeUTF8));
        label_32->setText(QApplication::translate("optionsWindow", "Personnages", 0, QApplication::UnicodeUTF8));
        buttonColoringCopyrights->setText(QApplication::translate("optionsWindow", "Choisir", 0, QApplication::UnicodeUTF8));
        buttonColoringCharacters->setText(QApplication::translate("optionsWindow", "Choisir", 0, QApplication::UnicodeUTF8));
        label_33->setText(QApplication::translate("optionsWindow", "Mod\303\250les", 0, QApplication::UnicodeUTF8));
        label_34->setText(QApplication::translate("optionsWindow", "G\303\251n\303\251raux", 0, QApplication::UnicodeUTF8));
        buttonColoringGenerals->setText(QApplication::translate("optionsWindow", "Choisir", 0, QApplication::UnicodeUTF8));
        buttonColoringModels->setText(QApplication::translate("optionsWindow", "Choisir", 0, QApplication::UnicodeUTF8));
        label_25->setText(QApplication::translate("optionsWindow", "Initialisation", 0, QApplication::UnicodeUTF8));
        label_26->setText(QApplication::translate("optionsWindow", "Image", 0, QApplication::UnicodeUTF8));
        label_27->setText(QApplication::translate("optionsWindow", "Symboles disponibles : les m\303\252mes que dans la partie sauvegarde.", 0, QApplication::UnicodeUTF8));
        label_28->setText(QApplication::translate("optionsWindow", "Tag", 0, QApplication::UnicodeUTF8));
        label_29->setText(QApplication::translate("optionsWindow", "Symboles disponibles : <i>%tag%</i>, <i>%type%</i>, <i>%number%</i>.<br/><i>%tag%</i> : le tag<br/><i>%type%</i> : type du tag, \"general\", \"artist\", \"copyright\", \"character\", \"model\" ou \"photo_set\"<br/><i>%number%</i> : le num\303\251ro du type de tag (varie entre 0 et 6)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class optionsWindow: public Ui_optionsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONSWINDOW_H
