#include "logging.h"
#include <QCoreApplication>
#include <QSettings>
#include <QSslSocket>
#include <QSysInfo>
#include <QString>
#include "logger.h"
#include "models/profile.h"


void logSystemInformation(Profile *profile)
{
	// Software
	log(QStringLiteral("Software version: %1.").arg(VERSION), Logger::Info);
	#ifdef NIGHTLY
		log(QStringLiteral("Nightly version: %1.").arg(QString(NIGHTLY_COMMIT)), Logger::Info);
	#endif

	// Hardware
	log(QStringLiteral("Software CPU architecture: %1.").arg(VERSION_PLATFORM), Logger::Info);
	#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
		log(QStringLiteral("Computer CPU architecture: %1.").arg(QSysInfo::currentCpuArchitecture()), Logger::Info);
		log(QStringLiteral("Qt CPU architecture: %1.").arg(QSysInfo::buildCpuArchitecture()), Logger::Info);
		log(QStringLiteral("Computer platform: %1.").arg(QSysInfo::prettyProductName()), Logger::Info);
	#endif

	// Paths
	log(QStringLiteral("Path: `%1`").arg(qApp->applicationDirPath()), Logger::Info);
	log(QStringLiteral("Loading preferences from `%1`").arg(profile->getSettings()->fileName()), Logger::Info);
	log(QStringLiteral("Temporary path: `%1`").arg(profile->tempPath()), Logger::Info);
	log(QStringLiteral("Sources found: %1").arg(profile->getSites().count()), Logger::Info);

	// SSL
	if (!QSslSocket::supportsSsl()) {
		log(QStringLiteral("Missing SSL libraries"), Logger::Error);
	} else {
		log(QStringLiteral("SSL libraries: %1").arg(QSslSocket::sslLibraryVersionString()), Logger::Info);
	}
}

QString logToHtml(const QString &msg)
{
	// Find meta stop characters
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

	return htmlMsg;
}
