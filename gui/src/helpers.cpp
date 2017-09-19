#include "helpers.h"
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QStringList>
#include <QDesktopServices>
#include <QUrl>


/**
 * Popup a message notifying the user that something went wrong.
 * @param	parent	The parent widget
 * @param	error	The error message
 */
void error(QWidget *parent, QString error)
{
	QMessageBox::critical(parent, QObject::tr("Error"), error);
}

/**
 * Opens the file explorer and select the file.
 * @param	pathIn	The path to the file.
 */
void showInGraphicalShell(const QString &pathIn)
{
	// Mac & Windows support folder or file.
	#if defined(Q_OS_WIN)
		QString param;
		if (!QFileInfo(pathIn).isDir())
		{ param = QLatin1String("/select,"); }
		param += QDir::toNativeSeparators(pathIn);
		QProcess::startDetached("explorer.exe "+param);
	#elif defined(Q_OS_MAC)
		// Q_UNUSED(parent)
		QStringList scriptArgs;
		scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(pathIn);
		QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
		scriptArgs.clear();
		scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
		QProcess::execute("/usr/bin/osascript", scriptArgs);
	#else
		QDesktopServices::openUrl(QUrl("file:///"+pathIn));
	#endif
}

void clearLayout(QLayout *layout)
{
	if (layout == nullptr)
		return;

	QLayoutItem *item;
	while ((item = layout->takeAt(0)))
	{
		if (item->layout())
		{
			clearLayout(item->layout());
			item->layout()->deleteLater();
		}
		item->widget()->deleteLater();
		delete item;
	}
}

QString parseMarkdown(QString str)
{
	// Windows EOL
	str.replace("\\r\\n", "\\n");

	// Headers
	QRegExp header("\\n(#+)([^#].*)\\n");
	header.setMinimal(true);
	int pos = 0;
	while ((pos = header.indexIn(str, pos)) != -1) {
		int level = qMax(1, qMin(6, header.cap(1).length()));
		QString result = "<h" + QString::number(level) + ">" + header.cap(2).trimmed() + "</h" + QString::number(level) + ">";
		str.replace(header.cap(0), result);
		pos += header.matchedLength();
	}

	// Issue links
	QRegExp issueLinks("(issue|fix) #(\\d+)");
	str.replace(issueLinks, "<a href='" + QString(PROJECT_GITHUB_URL) + "/issues/\\2'>\\1 #\\2</a>");

	// Line breaks to HTML
	str.replace("\n", "<br/>");

	return str;
}
