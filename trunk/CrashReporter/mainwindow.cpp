#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QSettings>
#include <QDebug>
#include <QTranslator>
#include "mainwindow.h"
#include "ui_mainwindow.h"



QString savePath(QString file)
{
	if (QFile(QDir::toNativeSeparators(qApp->applicationDirPath()+"/settings.ini")).exists())
	{ return QDir::toNativeSeparators(qApp->applicationDirPath()+"/"+file); }
	return QDir::toNativeSeparators(QDir::homePath()+"/Grabber/"+file);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_restart(false)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QString lang = settings.value("language", "English").toString();

	QLocale locale = QLocale(lang);
	QLocale::setDefault(locale);
	QTranslator *translator = new QTranslator(this);
	if (translator->load("crashreporter/"+lang))
	{ qApp->installTranslator(translator); }

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

void MainWindow::on_buttonRestart_clicked()
{
	m_restart = true;
	sendCrashReport();
}

void MainWindow::on_buttonQuit_clicked()
{
	sendCrashReport();
}

void MainWindow::sendCrashReport()
{
	if (ui->checkSend->isChecked())
	{ QDesktopServices::openUrl(QUrl("http://code.google.com/p/imgbrd-grabber/issues/entry")); }
	finished();
}
void MainWindow::finished()
{
	if (m_restart)
	{
		if (QFile::exists("Grabber.exe"))
		{ QProcess::startDetached("\"Grabber.exe\""); }
	}
	close();
}
