#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QMainWindow>
#include <QSettings>
#include <QTranslator>
#include "crash-reporter-window.h"
#include "ui_crash-reporter-window.h"


QString savePath(const QString &file, bool exists = false)
{
	#ifdef TEST
		Q_UNUSED(exists);
		return QDir::toNativeSeparators(QDir::currentPath()+"/tests/resources/"+file);
	#else
		const QString &check = exists ? file : "settings.ini";
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

CrashReporterWindow::CrashReporterWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::CrashReporterWindow), m_restart(false)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);

	// Translate UI
	const QString lang = settings.value("language", "English").toString();
	const QLocale locale = QLocale(lang);
	QLocale::setDefault(locale);
	auto *translator = new QTranslator(this);
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

CrashReporterWindow::~CrashReporterWindow()
{
	delete ui;
}

void CrashReporterWindow::restart()
{
	m_restart = true;
	sendCrashReport();
}

void CrashReporterWindow::sendCrashReport()
{
	if (ui->checkSend->isChecked())
	{
		QDesktopServices::openUrl(QUrl(QString(PROJECT_GITHUB_URL) + "/issues/new?labels[]=crash"));
	}

	finished();
}

void CrashReporterWindow::finished()
{
	if (m_restart && QFile::exists("Grabber.exe"))
	{
		QProcess::startDetached("\"Grabber.exe\"");
	}

	close();
}
