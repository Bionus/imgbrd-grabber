#include "tabs/log-tab.h"
#include <QDesktopServices>
#include <QRegularExpression>
#include <QScrollBar>
#include <ui_log-tab.h>
#include "logger.h"


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
	// Find meta delimitations
	QString htmlMsg = msg;
	int timeEnd = msg.indexOf(']');
	int levelEnd = msg.indexOf(']', timeEnd + 1);
	QString level = msg.mid(timeEnd + 2, levelEnd - timeEnd - 2);

	// Level color
	static const QMap<QString, QString> colors
	{
		{ "Debug", "#999" },
		{ "Info", "" },
		{ "Warning", "orange" },
		{ "Error", "red" },
	};
	QString levelColor = colors[level];
	if (!levelColor.isEmpty()) {
		htmlMsg.insert(msg.size(), "</span>");
		htmlMsg.insert(timeEnd + 1, QString("<span style='color:%1'>").arg(colors[level]));
	}

	// Time color
	htmlMsg.insert(timeEnd + 1, "</span>");
	htmlMsg.insert(0, "<span style='color:darkgreen'>");

	// Links color
	static const QRegularExpression rxLinks("`(http[^`]+)`");
	htmlMsg.replace(rxLinks, R"(<a href="\1">\1</a>)");

	// File paths color
	#ifdef Q_OS_WIN
		static const QRegularExpression rxPaths(R"(`(\w:[\\/][^`]+)`)");
	#else
		static const QRegularExpression rxPaths("`(/[^`]+)`");
	#endif
	htmlMsg.replace(rxPaths, R"(<a href="file:///\1">\1</a>)");

	ui->labelLog->appendHtml(htmlMsg);
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

void LogTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}
