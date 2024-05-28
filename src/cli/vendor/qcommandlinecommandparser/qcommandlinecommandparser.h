#ifndef QCOMMANDLINECOMMANDPARSER_H
#define QCOMMANDLINECOMMANDPARSER_H

#include <QCommandLineParser>
#include <QString>
#include <QStringList>
#include <QMap>


class QCommandLineCommandParser : public QCommandLineParser
{
	public:
		/**
		 * Same as `QCommandLineParser::setApplicationDescription` but with a newline prefix.
		 *
		 * @param description The description to use in the help text.
		 */
		void setApplicationDescription(const QString &description);

		/**
		 * Same as `QCommandLineParser::addOption` but also computes the maximum option name size.
		 *
		 * @param option The option to add to the parser.
		 * @return Whether the option could be added successfully.
		 */
		bool addOption(const QCommandLineOption &option);

		/**
		 * Same as `QCommandLineParser::addHelpOption` but without --help-all (for Qt options).
		 *
		 * @return The added option instance, which can be used to call isSet().
		 */
		QCommandLineOption addHelpOptionOnly();

		/**
		 * Same as `QCommandLineParser::addVersionOption` but also calculates the longest option name.
		 *
		 * @return The added option instance, which can be used to call isSet().
		 */
		QCommandLineOption addVersionOption();

		/**
		 * Same as `QCommandLineParser::helpText` but with overrides for:
		 * - Showing the command prefix in the "usage" part
		 * - Showing the command options in the "usage part"
		 * - Showing the list of commands at the end of the help text
		 *
		 * @return A string containing the complete help information.
		 */
		QString helpText() const;

		/**
		 * Same as `QCommandLineParser::showHelp` but using the overriden help text.
		 *
		 * @param exitCode The exit code to use when exiting the application after showing the help text.
		 */
		Q_NORETURN void showHelp(int exitCode = 0);

		/**
		 * Add a new command to this parser.
		 *
		 * @param name The name of the command .
		 * @param description The short description of the command, used in the help text.
		 * @return Whether the command was added successfully. It will fail if the name is already taken, for example.
		 */
		bool addCommand(const QString &name, const QString &description);

		/**
		 * After parsing is done, will contain the command chosen by the user.
		 *
		 * @return The command chosen by the user if found, an empty string otherwise.
		 */
		QString command() const;

		/**
		 * Sets the prefix of this parser. The prefix used in two places:
		 * - The help text, to show directly after the program name
		 * - In positionalArguments() to only return this parser's arguments without the prefix
		 *
		 * @param prefix The prefix to use, as a list of command names.
		 */
		void setPrefix(QStringList prefix);

		/**
		 * Same as `QCommandLineParser::positionalArguments()` but excluding the parser's prefix.
		 *
		 * @return The list of positional arguments found during parsing.
		 */
		QStringList positionalArguments() const;

		// We hide the process() methods to not fail on unknown sub-command options
		void process(const QStringList &arguments);
		void process(const QCoreApplication &app);

	protected:
		void calculateLongestOptionName(const QCommandLineOption &option);

	private:
		/**
		 * Whether the help option was added via addHelpOptionOnly().
		 */
		bool m_helpOptionOnly = false;

		/**
		 * The longest option name in this parser, used for column wrapping in the help text.
		 */
		int m_longestOptionName = 0;

		/**
		 * The parser's command prefix.
		 */
		QStringList m_prefix;

		/**
		 * The list of commands added to this parser.
		 */
		QMap<QString, QString> m_commands;
};

#endif // QCOMMANDLINECOMMANDPARSER_H
