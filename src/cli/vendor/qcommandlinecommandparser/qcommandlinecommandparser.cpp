#include "qcommandlinecommandparser.h"
#include <QDebug>
#include <QString>
#include "vendor.h"

Q_CORE_EXPORT void qt_call_post_routines();


void QCommandLineCommandParser::setApplicationDescription(const QString &description)
{
	QCommandLineParser::setApplicationDescription(u'\n' + description);
}

bool QCommandLineCommandParser::addOption(const QCommandLineOption &option)
{
	calculateLongestOptionName(option);
	return QCommandLineParser::addOption(option);
}

void QCommandLineCommandParser::calculateLongestOptionName(const QCommandLineOption &option)
{
	m_longestOptionName = qMax(m_longestOptionName, int(optionName(option).size()));
}

QCommandLineOption QCommandLineCommandParser::addHelpOptionOnly()
{
	const QStringList names {
		#ifdef Q_OS_WIN
			QStringLiteral("?"),
		#endif
		QStringLiteral("h"),
		QStringLiteral("help")
	};
	QCommandLineOption opt(names, QCommandLineParser::tr("Displays help on commandline options."));
	addOption(opt);
	m_helpOptionOnly = true;
	return opt;
}

QCommandLineOption QCommandLineCommandParser::addVersionOption()
{
	const QCommandLineOption opt = QCommandLineParser::addVersionOption();
	calculateLongestOptionName(opt);
	return opt;
}

QString QCommandLineCommandParser::helpText() const
{
	// Useful values
	const QLatin1Char nl('\n');
	const QString optionsArg = u' ' + QCommandLineParser::tr("[options]");

	// Use the default Qt help text as a base
	QString text = QCommandLineParser::helpText();

	// Add the prefix to the left of the options
	text.replace(optionsArg, (m_prefix.isEmpty() ? "" : " " + m_prefix.join(' ')) + optionsArg + (m_commands.isEmpty() ? "" : " <command> [args]"));

	// Add the commands at the end
	if (!m_commands.isEmpty()) {
		const int columnMaxWidth = qMin(50, m_longestOptionName + 1);

		text += nl + QCommandLineParser::tr("Commands:") + nl;
		for (auto it = m_commands.constBegin(); it != m_commands.constEnd(); ++it) {
			text += wrapText(it.key(), columnMaxWidth, it.value());
		}
	}

	return text;
}

void QCommandLineCommandParser::showHelp(int exitCode)
{
	showParserMessage(helpText(), UsageMessage);
	qt_call_post_routines();
	::exit(exitCode);
}

bool QCommandLineCommandParser::addCommand(const QString &name, const QString &description/*, SuperParser *parser*/)
{
	if (m_commands.contains(name)) {
		qWarning() << "QCommandLineCommandParser: already having a command named" << name;
		return false;
	}

	m_commands.insert(name, description);
	return true;
}

QString QCommandLineCommandParser::command() const
{
	// Early return if no commands have been set
	if (m_commands.isEmpty()) {
		return {};
	}

	// Early return if no arguments were passed
	const QStringList args = positionalArguments();
	if (args.isEmpty()) {
		return {};
	}

	// Only return valid command names
	const QString &command = args.first();
	if (!m_commands.contains(command)) {
		return {};
	}

	return command;
}

void QCommandLineCommandParser::setPrefix(QStringList prefix)
{
	m_prefix = std::move(prefix);
}

QStringList QCommandLineCommandParser::positionalArguments() const
{
	const QStringList args = QCommandLineParser::positionalArguments();

	// Ensure the positional arguments start with the prefix, if found
	for (int i = 0; i < m_prefix.count(); ++i) {
		if (args.length() <= i || args[i] != m_prefix[i]) {
			qWarning() << "QCommandLineCommandParser: prefix mismatch in positional arguments";
			return {};
		}
	}

	return args.sliced(m_prefix.count());
}

void QCommandLineCommandParser::process(const QStringList &arguments)
{
	if (!m_commands.isEmpty()) {
		QCommandLineParser::parse(arguments);
	} else {
		QCommandLineParser::process(arguments);
	}

	// Only show the help if the command is empty, as sub-commands could also implement a -h or --help option
	if (m_helpOptionOnly && isSet(QStringLiteral("help")) && command().isEmpty()) {
		showHelp();
	}
}

void QCommandLineCommandParser::process(const QCoreApplication &app)
{
	Q_UNUSED(app);
	process(QCoreApplication::arguments());
}
