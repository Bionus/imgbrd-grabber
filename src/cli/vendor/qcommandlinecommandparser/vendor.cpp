#include "vendor.h"

#include <QCoreApplication>
#include <QVariant>
#if defined(Q_OS_WIN) && !defined(QT_BOOTSTRAPPED)
#  include <qt_windows.h>
#endif

using namespace Qt::StringLiterals;


// Taken as-is from `QCommandLineParser`
#if defined(Q_OS_WIN) && !defined(QT_BOOTSTRAPPED)
// Return whether to use a message box. Use handles if a console can be obtained
// or we are run with redirected handles (for example, by QProcess).
static inline bool displayMessageBox()
{
	if (GetConsoleWindow()
		|| qEnvironmentVariableIsSet("QT_COMMAND_LINE_PARSER_NO_GUI_MESSAGE_BOXES"))
		return false;
	STARTUPINFO startupInfo;
	startupInfo.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&startupInfo);
	return !(startupInfo.dwFlags & STARTF_USESTDHANDLES);
}
#endif // Q_OS_WIN && !QT_BOOTSTRAPPED

// Taken as-is from `QCommandLineParser`
void showParserMessage(const QString &message, MessageType type)
{
#if defined(Q_OS_WIN) && !defined(QT_BOOTSTRAPPED)
	if (displayMessageBox()) {
		const UINT flags = MB_OK | MB_TOPMOST | MB_SETFOREGROUND
						   | (type == UsageMessage ? MB_ICONINFORMATION : MB_ICONERROR);
		QString title;
		if (QCoreApplication::instance())
			title = QCoreApplication::instance()->property("applicationDisplayName").toString();
		if (title.isEmpty())
			title = QCoreApplication::applicationName();
		MessageBoxW(0, reinterpret_cast<const wchar_t *>(message.utf16()),
					reinterpret_cast<const wchar_t *>(title.utf16()), flags);
		return;
	}
#endif // Q_OS_WIN && !QT_BOOTSTRAPPED
	fputs(qPrintable(message), type == UsageMessage ? stdout : stderr);
}

// Taken as-is from `QCommandLineParser`
QString wrapText(const QString &names, int optionNameMaxWidth, const QString &description)
{
	const auto nl = u'\n';
	const auto indentation = "  "_L1;

	// In case the list of option names is very long, wrap it as well
	int nameIndex = 0;
	auto nextNameSection = [&]() {
		QString section = names.mid(nameIndex, optionNameMaxWidth);
		nameIndex += section.size();
		return section;
	};

	QString text;
	qsizetype lineStart = 0;
	qsizetype lastBreakable = -1;
	const int max = 79 - (indentation.size() + optionNameMaxWidth + 1);
	int x = 0;
	const qsizetype len = description.size();

	for (qsizetype i = 0; i < len; ++i) {
		++x;
		const QChar c = description.at(i);
		if (c.isSpace())
			lastBreakable = i;

		qsizetype breakAt = -1;
		qsizetype nextLineStart = -1;
		if (x > max && lastBreakable != -1) {
			// time to break and we know where
			breakAt = lastBreakable;
			nextLineStart = lastBreakable + 1;
		} else if ((x > max - 1 && lastBreakable == -1) || i == len - 1) {
			// time to break but found nowhere [-> break here], or end of last line
			breakAt = i + 1;
			nextLineStart = breakAt;
		} else if (c == nl) {
			// forced break
			breakAt = i;
			nextLineStart = i + 1;
		}

		if (breakAt != -1) {
			const qsizetype numChars = breakAt - lineStart;
			//qDebug() << "breakAt=" << description.at(breakAt) << "breakAtSpace=" << breakAtSpace << lineStart << "to" << breakAt << description.mid(lineStart, numChars);
			text += indentation + nextNameSection().leftJustified(optionNameMaxWidth) + u' ';
			text += QStringView{description}.mid(lineStart, numChars);
			text += nl;
			x = 0;
			lastBreakable = -1;
			lineStart = nextLineStart;
			if (lineStart < len && description.at(lineStart).isSpace())
				++lineStart; // don't start a line with a space
			i = lineStart;
		}
	}

	while (nameIndex < names.size()) {
		text += indentation + nextNameSection() + nl;
	}

	return text;
}

// Taken and adapted from `QCommandLineParserPrivate::helpText`
QString optionName(const QCommandLineOption &option)
{
	if (option.flags() & QCommandLineOption::HiddenFromHelp) {
		return {};
	}

	const QStringList optionNames = option.names();
	QString optionNamesString;
	for (const QString &optionName : optionNames) {
		const int numDashes = optionName.size() == 1 ? 1 : 2;
		optionNamesString += QLatin1StringView("--", numDashes) + optionName + ", "_L1;
	}
	if (!optionNames.isEmpty())
		optionNamesString.chop(2); // remove trailing ", "
	const auto valueName = option.valueName();
	if (!valueName.isEmpty())
		optionNamesString += " <"_L1 + valueName + u'>';
	return optionNamesString;
}