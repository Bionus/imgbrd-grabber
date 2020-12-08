#include "tabs/log-tab.h"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QScrollBar>
#include <ui_log-tab.h>
#include "functions.h"
#include "helpers.h"
#include "logger.h"
#include "utils/logging.h"


LogTab::LogTab(QWidget *parent)
	: QWidget(parent), ui(new Ui::LogTab)
{
	ui->setupUi(this);

	// Load already written log
	QFile logFile(Logger::getInstance().logFile());
	if (logFile.open(QFile::ReadOnly | QFile::Text)) {
		while (!logFile.atEnd()) {
			write(logFile.readLine());
		}
		logFile.close();
	}

	connect(&Logger::getInstance(), &Logger::newLog, this, &LogTab::write);
}

LogTab::~LogTab()
{
	close();
	delete ui;
}

void LogTab::write(const QString &msg)
{
	ui->labelLog->appendHtml(logToHtml(msg));
	ui->labelLog->verticalScrollBar()->setValue(ui->labelLog->verticalScrollBar()->maximum());
}

void LogTab::clear()
{
	QFile logFile(Logger::getInstance().logFile());
	if (logFile.open(QFile::WriteOnly | QFile::Text)) {
		logFile.resize(0);
		logFile.close();
	}

	ui->labelLog->clear();
}

void LogTab::open()
{
	QDesktopServices::openUrl("file:///" + Logger::getInstance().logFile());
}

void LogTab::openDir()
{
	QDir dir(savePath());
	if (dir.exists()) {
		showInGraphicalShell(dir.absolutePath());
	}
}

void LogTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}
