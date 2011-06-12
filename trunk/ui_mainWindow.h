/********************************************************************************
** Form generated from reading UI file 'mainWindow.ui'
**
** Created: Sun 12. Jun 16:34:40 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDateEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainWindow
{
public:
    QAction *actionOptions;
    QAction *actionFolder;
    QAction *actionQuit;
    QAction *actionHelp;
    QAction *actionAboutGrabber;
    QAction *actionAboutQt;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QVBoxLayout *layoutExplore;
    QGridLayout *layoutFields;
    QLabel *label;
    QLabel *label_2;
    QPushButton *buttonCalendar;
    QPushButton *buttonTags;
    QPushButton *buttonPopular;
    QDateEdit *datePopular;
    QSpinBox *spinPage;
    QPushButton *pushButton;
    QWidget *widgetPlus;
    QGridLayout *gridLayout;
    QLabel *label_6;
    QLabel *label_7;
    QSpinBox *spinImagesPerPage;
    QSpinBox *spinColumns;
    QGridLayout *layoutResults;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonSourcesList;
    QCheckBox *checkMergeResults;
    QLabel *labelMergeResults;
    QPushButton *buttonGetpage;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_4;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QComboBox *comboOrderfavorites;
    QComboBox *comboOrderasc;
    QGridLayout *layoutFavorites;
    QGridLayout *layoutFavoritesResults;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *buttonSourcesFavs;
    QPushButton *buttonBack;
    QPushButton *buttonMarkasviewed;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_6;
    QWidget *widget_4;
    QVBoxLayout *verticalLayout_10;
    QLabel *label_4;
    QTableWidget *tableBatchGroups;
    QPushButton *pushButton_12;
    QLabel *label_5;
    QTableWidget *tableBatchUniques;
    QPushButton *pushButton_13;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton_14;
    QPushButton *pushButton_15;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_9;
    QWidget *widget_3;
    QVBoxLayout *verticalLayout_7;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_8;
    QLabel *labelLog;
    QPushButton *buttonClearlog;
    QMenuBar *menubar;
    QMenu *menuOptions;
    QMenu *menu_propos;

    void setupUi(QMainWindow *mainWindow)
    {
        if (mainWindow->objectName().isEmpty())
            mainWindow->setObjectName(QString::fromUtf8("mainWindow"));
        mainWindow->resize(800, 600);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        mainWindow->setWindowIcon(icon);
        actionOptions = new QAction(mainWindow);
        actionOptions->setObjectName(QString::fromUtf8("actionOptions"));
        actionFolder = new QAction(mainWindow);
        actionFolder->setObjectName(QString::fromUtf8("actionFolder"));
        actionQuit = new QAction(mainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionHelp = new QAction(mainWindow);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionAboutGrabber = new QAction(mainWindow);
        actionAboutGrabber->setObjectName(QString::fromUtf8("actionAboutGrabber"));
        actionAboutQt = new QAction(mainWindow);
        actionAboutQt->setObjectName(QString::fromUtf8("actionAboutQt"));
        centralwidget = new QWidget(mainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
#ifndef Q_OS_MAC
        verticalLayout->setSpacing(6);
#endif
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        widget = new QWidget(tab);
        widget->setObjectName(QString::fromUtf8("widget"));
        layoutExplore = new QVBoxLayout(widget);
        layoutExplore->setContentsMargins(0, 0, 0, 0);
        layoutExplore->setObjectName(QString::fromUtf8("layoutExplore"));
        layoutFields = new QGridLayout();
#ifndef Q_OS_MAC
        layoutFields->setContentsMargins(0, 0, 0, 0);
#endif
        layoutFields->setObjectName(QString::fromUtf8("layoutFields"));
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));

        layoutFields->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        layoutFields->addWidget(label_2, 1, 0, 1, 1);

        buttonCalendar = new QPushButton(widget);
        buttonCalendar->setObjectName(QString::fromUtf8("buttonCalendar"));

        layoutFields->addWidget(buttonCalendar, 1, 2, 1, 1);

        buttonTags = new QPushButton(widget);
        buttonTags->setObjectName(QString::fromUtf8("buttonTags"));

        layoutFields->addWidget(buttonTags, 0, 5, 1, 1);

        buttonPopular = new QPushButton(widget);
        buttonPopular->setObjectName(QString::fromUtf8("buttonPopular"));

        layoutFields->addWidget(buttonPopular, 1, 5, 1, 1);

        datePopular = new QDateEdit(widget);
        datePopular->setObjectName(QString::fromUtf8("datePopular"));

        layoutFields->addWidget(datePopular, 1, 1, 1, 1);

        spinPage = new QSpinBox(widget);
        spinPage->setObjectName(QString::fromUtf8("spinPage"));
        spinPage->setMinimumSize(QSize(60, 0));
        spinPage->setMinimum(1);
        spinPage->setMaximum(1000);

        layoutFields->addWidget(spinPage, 0, 3, 2, 1);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy1);
        pushButton->setMaximumSize(QSize(30, 16777215));
        pushButton->setCheckable(true);

        layoutFields->addWidget(pushButton, 0, 6, 2, 1);

        layoutFields->setColumnStretch(1, 1);

        layoutExplore->addLayout(layoutFields);

        widgetPlus = new QWidget(widget);
        widgetPlus->setObjectName(QString::fromUtf8("widgetPlus"));
        widgetPlus->setEnabled(true);
        gridLayout = new QGridLayout(widgetPlus);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_6 = new QLabel(widgetPlus);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 0, 0, 1, 1);

        label_7 = new QLabel(widgetPlus);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 0, 2, 1, 1);

        spinImagesPerPage = new QSpinBox(widgetPlus);
        spinImagesPerPage->setObjectName(QString::fromUtf8("spinImagesPerPage"));
        spinImagesPerPage->setMinimum(1);
        spinImagesPerPage->setMaximum(1000);
        spinImagesPerPage->setValue(20);

        gridLayout->addWidget(spinImagesPerPage, 0, 1, 1, 1);

        spinColumns = new QSpinBox(widgetPlus);
        spinColumns->setObjectName(QString::fromUtf8("spinColumns"));
        spinColumns->setMinimum(1);
        spinColumns->setMaximum(10);

        gridLayout->addWidget(spinColumns, 0, 3, 1, 1);


        layoutExplore->addWidget(widgetPlus);

        layoutResults = new QGridLayout();
        layoutResults->setObjectName(QString::fromUtf8("layoutResults"));

        layoutExplore->addLayout(layoutResults);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        layoutExplore->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonSourcesList = new QPushButton(widget);
        buttonSourcesList->setObjectName(QString::fromUtf8("buttonSourcesList"));

        horizontalLayout->addWidget(buttonSourcesList);

        checkMergeResults = new QCheckBox(widget);
        checkMergeResults->setObjectName(QString::fromUtf8("checkMergeResults"));
        checkMergeResults->setChecked(true);

        horizontalLayout->addWidget(checkMergeResults);

        labelMergeResults = new QLabel(widget);
        labelMergeResults->setObjectName(QString::fromUtf8("labelMergeResults"));

        horizontalLayout->addWidget(labelMergeResults);

        buttonGetpage = new QPushButton(widget);
        buttonGetpage->setObjectName(QString::fromUtf8("buttonGetpage"));

        horizontalLayout->addWidget(buttonGetpage);


        layoutExplore->addLayout(horizontalLayout);

        layoutExplore->setStretch(2, 1);

        verticalLayout_2->addWidget(widget);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_4 = new QVBoxLayout(tab_2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        widget_2 = new QWidget(tab_2);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        verticalLayout_5 = new QVBoxLayout(widget_2);
#ifndef Q_OS_MAC
        verticalLayout_5->setSpacing(6);
#endif
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_3 = new QLabel(widget_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(label_3);

        comboOrderfavorites = new QComboBox(widget_2);
        comboOrderfavorites->setObjectName(QString::fromUtf8("comboOrderfavorites"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(comboOrderfavorites->sizePolicy().hasHeightForWidth());
        comboOrderfavorites->setSizePolicy(sizePolicy2);

        horizontalLayout_4->addWidget(comboOrderfavorites);

        comboOrderasc = new QComboBox(widget_2);
        comboOrderasc->setObjectName(QString::fromUtf8("comboOrderasc"));

        horizontalLayout_4->addWidget(comboOrderasc);


        verticalLayout_5->addLayout(horizontalLayout_4);

        layoutFavorites = new QGridLayout();
        layoutFavorites->setObjectName(QString::fromUtf8("layoutFavorites"));

        verticalLayout_5->addLayout(layoutFavorites);

        layoutFavoritesResults = new QGridLayout();
        layoutFavoritesResults->setObjectName(QString::fromUtf8("layoutFavoritesResults"));

        verticalLayout_5->addLayout(layoutFavoritesResults);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        buttonSourcesFavs = new QPushButton(widget_2);
        buttonSourcesFavs->setObjectName(QString::fromUtf8("buttonSourcesFavs"));

        horizontalLayout_2->addWidget(buttonSourcesFavs);

        buttonBack = new QPushButton(widget_2);
        buttonBack->setObjectName(QString::fromUtf8("buttonBack"));

        horizontalLayout_2->addWidget(buttonBack);

        buttonMarkasviewed = new QPushButton(widget_2);
        buttonMarkasviewed->setObjectName(QString::fromUtf8("buttonMarkasviewed"));

        horizontalLayout_2->addWidget(buttonMarkasviewed);


        verticalLayout_5->addLayout(horizontalLayout_2);

        verticalLayout_5->setStretch(1, 1);

        verticalLayout_4->addWidget(widget_2);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout_6 = new QVBoxLayout(tab_3);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        widget_4 = new QWidget(tab_3);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        verticalLayout_10 = new QVBoxLayout(widget_4);
        verticalLayout_10->setContentsMargins(0, 0, 0, 0);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        label_4 = new QLabel(widget_4);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_10->addWidget(label_4);

        tableBatchGroups = new QTableWidget(widget_4);
        if (tableBatchGroups->columnCount() < 9)
            tableBatchGroups->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableBatchGroups->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        tableBatchGroups->setObjectName(QString::fromUtf8("tableBatchGroups"));

        verticalLayout_10->addWidget(tableBatchGroups);

        pushButton_12 = new QPushButton(widget_4);
        pushButton_12->setObjectName(QString::fromUtf8("pushButton_12"));

        verticalLayout_10->addWidget(pushButton_12);

        label_5 = new QLabel(widget_4);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_10->addWidget(label_5);

        tableBatchUniques = new QTableWidget(widget_4);
        if (tableBatchUniques->columnCount() < 6)
            tableBatchUniques->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableBatchUniques->setHorizontalHeaderItem(0, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableBatchUniques->setHorizontalHeaderItem(1, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableBatchUniques->setHorizontalHeaderItem(2, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableBatchUniques->setHorizontalHeaderItem(3, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableBatchUniques->setHorizontalHeaderItem(4, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableBatchUniques->setHorizontalHeaderItem(5, __qtablewidgetitem14);
        tableBatchUniques->setObjectName(QString::fromUtf8("tableBatchUniques"));

        verticalLayout_10->addWidget(tableBatchUniques);

        pushButton_13 = new QPushButton(widget_4);
        pushButton_13->setObjectName(QString::fromUtf8("pushButton_13"));

        verticalLayout_10->addWidget(pushButton_13);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButton_14 = new QPushButton(widget_4);
        pushButton_14->setObjectName(QString::fromUtf8("pushButton_14"));

        horizontalLayout_3->addWidget(pushButton_14);

        pushButton_15 = new QPushButton(widget_4);
        pushButton_15->setObjectName(QString::fromUtf8("pushButton_15"));

        horizontalLayout_3->addWidget(pushButton_15);


        verticalLayout_10->addLayout(horizontalLayout_3);


        verticalLayout_6->addWidget(widget_4);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        verticalLayout_9 = new QVBoxLayout(tab_4);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        widget_3 = new QWidget(tab_4);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        verticalLayout_7 = new QVBoxLayout(widget_3);
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        scrollArea = new QScrollArea(widget_3);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 774, 504));
        verticalLayout_8 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_8->setContentsMargins(6, 6, 6, 6);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        labelLog = new QLabel(scrollAreaWidgetContents);
        labelLog->setObjectName(QString::fromUtf8("labelLog"));
        labelLog->setTextFormat(Qt::RichText);
        labelLog->setOpenExternalLinks(true);

        verticalLayout_8->addWidget(labelLog);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_7->addWidget(scrollArea);

        buttonClearlog = new QPushButton(widget_3);
        buttonClearlog->setObjectName(QString::fromUtf8("buttonClearlog"));

        verticalLayout_7->addWidget(buttonClearlog);


        verticalLayout_9->addWidget(widget_3);

        tabWidget->addTab(tab_4, QString());

        verticalLayout->addWidget(tabWidget);

        mainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(mainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuOptions = new QMenu(menubar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        menu_propos = new QMenu(menubar);
        menu_propos->setObjectName(QString::fromUtf8("menu_propos"));
        mainWindow->setMenuBar(menubar);

        menubar->addAction(menuOptions->menuAction());
        menubar->addAction(menu_propos->menuAction());
        menuOptions->addAction(actionOptions);
        menuOptions->addAction(actionFolder);
        menuOptions->addAction(actionQuit);
        menu_propos->addAction(actionHelp);
        menu_propos->addSeparator();
        menu_propos->addAction(actionAboutGrabber);
        menu_propos->addAction(actionAboutQt);

        retranslateUi(mainWindow);
        QObject::connect(actionHelp, SIGNAL(triggered()), mainWindow, SLOT(help()));
        QObject::connect(actionOptions, SIGNAL(triggered()), mainWindow, SLOT(options()));
        QObject::connect(actionAboutGrabber, SIGNAL(triggered()), mainWindow, SLOT(aboutAuthor()));
        QObject::connect(buttonSourcesList, SIGNAL(clicked()), mainWindow, SLOT(advanced()));
        QObject::connect(comboOrderfavorites, SIGNAL(currentIndexChanged(int)), mainWindow, SLOT(updateFavorites()));
        QObject::connect(comboOrderasc, SIGNAL(currentIndexChanged(int)), mainWindow, SLOT(updateFavorites()));
        QObject::connect(buttonTags, SIGNAL(clicked()), mainWindow, SLOT(webUpdateTags()));
        QObject::connect(buttonPopular, SIGNAL(clicked()), mainWindow, SLOT(webUpdatePopular()));
        QObject::connect(buttonSourcesFavs, SIGNAL(clicked()), mainWindow, SLOT(advanced()));
        QObject::connect(pushButton_15, SIGNAL(clicked()), mainWindow, SLOT(batchClear()));
        QObject::connect(pushButton_12, SIGNAL(clicked()), mainWindow, SLOT(addGroup()));
        QObject::connect(pushButton_13, SIGNAL(clicked()), mainWindow, SLOT(addUnique()));
        QObject::connect(buttonClearlog, SIGNAL(clicked()), mainWindow, SLOT(logClear()));
        QObject::connect(buttonGetpage, SIGNAL(clicked()), mainWindow, SLOT(getPage()));
        QObject::connect(pushButton_14, SIGNAL(clicked()), mainWindow, SLOT(getAll()));
        QObject::connect(buttonBack, SIGNAL(clicked()), mainWindow, SLOT(favoritesBack()));
        QObject::connect(buttonMarkasviewed, SIGNAL(clicked()), mainWindow, SLOT(viewed()));
        QObject::connect(tableBatchGroups, SIGNAL(cellChanged(int,int)), mainWindow, SLOT(updateBatchGroups(int,int)));
        QObject::connect(actionFolder, SIGNAL(triggered()), mainWindow, SLOT(saveFolder()));
        QObject::connect(pushButton, SIGNAL(clicked()), mainWindow, SLOT(widgetPlusChange()));

        tabWidget->setCurrentIndex(0);
        comboOrderfavorites->setCurrentIndex(0);
        comboOrderasc->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(mainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *mainWindow)
    {
        mainWindow->setWindowTitle(QApplication::translate("mainWindow", "Grabber", 0, QApplication::UnicodeUTF8));
        actionOptions->setText(QApplication::translate("mainWindow", "Options", 0, QApplication::UnicodeUTF8));
        actionFolder->setText(QApplication::translate("mainWindow", "Dossier de sauvegarde", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("mainWindow", "Quitter", 0, QApplication::UnicodeUTF8));
        actionHelp->setText(QApplication::translate("mainWindow", "Aide", 0, QApplication::UnicodeUTF8));
        actionAboutGrabber->setText(QApplication::translate("mainWindow", "\303\200 propos de Grabber", 0, QApplication::UnicodeUTF8));
        actionAboutQt->setText(QApplication::translate("mainWindow", "\303\200 propos de Qt", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("mainWindow", "Liste", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("mainWindow", "Populaires", 0, QApplication::UnicodeUTF8));
        buttonCalendar->setText(QApplication::translate("mainWindow", "Choisir", 0, QApplication::UnicodeUTF8));
        buttonTags->setText(QApplication::translate("mainWindow", "Ok", 0, QApplication::UnicodeUTF8));
        buttonPopular->setText(QApplication::translate("mainWindow", "Ok", 0, QApplication::UnicodeUTF8));
        datePopular->setDisplayFormat(QApplication::translate("mainWindow", "dd/MM/yyyy", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("mainWindow", "+", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("mainWindow", "Images par page", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("mainWindow", "Nombre de colonnes", 0, QApplication::UnicodeUTF8));
        buttonSourcesList->setText(QApplication::translate("mainWindow", "Sources", 0, QApplication::UnicodeUTF8));
        checkMergeResults->setText(QApplication::translate("mainWindow", "Fusionner les r\303\251sultats", 0, QApplication::UnicodeUTF8));
        labelMergeResults->setText(QString());
        buttonGetpage->setText(QApplication::translate("mainWindow", "Prendre cette page", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("mainWindow", "Explorer", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("mainWindow", "Trier par", 0, QApplication::UnicodeUTF8));
        comboOrderfavorites->clear();
        comboOrderfavorites->insertItems(0, QStringList()
         << QApplication::translate("mainWindow", "Nom", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("mainWindow", "Note", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("mainWindow", "Derni\303\250re vue", 0, QApplication::UnicodeUTF8)
        );
        comboOrderasc->clear();
        comboOrderasc->insertItems(0, QStringList()
         << QApplication::translate("mainWindow", "Ascendant", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("mainWindow", "Descendant", 0, QApplication::UnicodeUTF8)
        );
        buttonSourcesFavs->setText(QApplication::translate("mainWindow", "Sources", 0, QApplication::UnicodeUTF8));
        buttonBack->setText(QApplication::translate("mainWindow", "Retour", 0, QApplication::UnicodeUTF8));
        buttonMarkasviewed->setText(QApplication::translate("mainWindow", "Marquer comme vu", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("mainWindow", "Favoris", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("mainWindow", "Groupes", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableBatchGroups->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("mainWindow", "Tags", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableBatchGroups->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("mainWindow", "Page", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableBatchGroups->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("mainWindow", "Images par page", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableBatchGroups->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("mainWindow", "Limite d'images", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableBatchGroups->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("mainWindow", "T\303\251l\303\251charger les images de la liste noire", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tableBatchGroups->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("mainWindow", "Source", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tableBatchGroups->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("mainWindow", "Populaires", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tableBatchGroups->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("mainWindow", "Noms de fichiers", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = tableBatchGroups->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QApplication::translate("mainWindow", "Dossier", 0, QApplication::UnicodeUTF8));
        pushButton_12->setText(QApplication::translate("mainWindow", "Ajouter", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("mainWindow", "Images seules", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = tableBatchUniques->horizontalHeaderItem(0);
        ___qtablewidgetitem9->setText(QApplication::translate("mainWindow", "Id", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = tableBatchUniques->horizontalHeaderItem(1);
        ___qtablewidgetitem10->setText(QApplication::translate("mainWindow", "Md5", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = tableBatchUniques->horizontalHeaderItem(2);
        ___qtablewidgetitem11->setText(QApplication::translate("mainWindow", "Classe", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = tableBatchUniques->horizontalHeaderItem(3);
        ___qtablewidgetitem12->setText(QApplication::translate("mainWindow", "Tags", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = tableBatchUniques->horizontalHeaderItem(4);
        ___qtablewidgetitem13->setText(QApplication::translate("mainWindow", "Url", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = tableBatchUniques->horizontalHeaderItem(5);
        ___qtablewidgetitem14->setText(QApplication::translate("mainWindow", "Site", 0, QApplication::UnicodeUTF8));
        pushButton_13->setText(QApplication::translate("mainWindow", "Ajouter", 0, QApplication::UnicodeUTF8));
        pushButton_14->setText(QApplication::translate("mainWindow", "Tout t\303\251l\303\251charger", 0, QApplication::UnicodeUTF8));
        pushButton_15->setText(QApplication::translate("mainWindow", "Effacer", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("mainWindow", "T\303\251l\303\251charger", 0, QApplication::UnicodeUTF8));
        labelLog->setText(QString());
        buttonClearlog->setText(QApplication::translate("mainWindow", "Effacer le log", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("mainWindow", "Log", 0, QApplication::UnicodeUTF8));
        menuOptions->setTitle(QApplication::translate("mainWindow", "Fichier", 0, QApplication::UnicodeUTF8));
        menu_propos->setTitle(QApplication::translate("mainWindow", "Aide", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mainWindow: public Ui_mainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
