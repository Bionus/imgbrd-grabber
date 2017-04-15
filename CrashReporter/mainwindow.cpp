#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QSettings>
#include <QTranslator>
#include "mainwindow.h"
#include "ui_mainwindow.h"


QString savePath(QString file, bool exists = false)
{
	#ifdef TEST
		Q_UNUSED(exists);
		return QDir::toNativeSeparators(QDir::currentPath()+"/tests/resources/"+file);
	#else
		QString check = exists ? file : "settings.ini";
		if (QFile(QDir::toNativeSeparators(qApp->applicationDirPath()+"/"+check)).exists())
		{ return QDir::toNativeSeparators(qApp->applicationDirPath()+"/"+file); }
		if (QFile(QDir::toNativeSeparators(QDir::currentPath()+"/"+check)).exists())
		{ return QDir::toNativeSeparators(QDir::currentPath()+"/"+file); }
		if (QFile(QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+check)).exists())
		{ return QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+file); }
		#ifdef __linux__
			if (QFile(QDir::toNativeSeparators(QDir::homePath()+"/.Grabber/"+check)).exists())
			{ return QDir::toNativeSeparators(QDir::homePath()+"/.Grabber/"+file); }
		#endif
		return QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"+file);
	#endif
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_restart(false)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);

	// Translate UI
	QString lang = settings.value("language", "English").toString();
	QLocale locale = QLocale(lang);
	QLocale::setDefault(locale);
	QTranslator *translator = new QTranslator(this);
	if (translator->load("crashreporter/"+lang))
	{
		qApp->installTranslator(translator);
	}

	// Setup UI with correct fields value
	ui->setupUi(this);
	ui->lineSettings->setText(savePath("settings.ini"));
	ui->lineLog->setText(savePath("main.log"));
	QFile f(savePath("lastdump"));
	if (f.exists())
	{
		if (f.open(QFile::ReadOnly))
		{
			ui->lineDump->setText(f.readAll());
			f.close();
		}
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::restart()
{
	m_restart = true;
	sendCrashReport();
}

void MainWindow::sendCrashReport()
{
	if (ui->checkSend->isChecked())
	{
		QDesktopServices::openUrl(QUrl(QString(PROJECT_GITHUB_URL) + "/issues/new?labels[]=crash"));
	}

	finished();
}

void MainWindow::finished()
{
	if (m_restart && QFile::exists("Grabber.exe"))
	{
		QProcess::startDetached("\"Grabber.exe\"");
	}

	close();
}
