#include "theme-loader.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QScreen>
#include <QSettings>


ThemeLoader::ThemeLoader(QString path, QSettings *settings, QObject *parent)
	: QObject(parent), m_path(std::move(path)), m_settings(settings)
{
	connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &ThemeLoader::themeFileChanged);
}

QStringList ThemeLoader::getAllThemes() const
{
	return QDir(m_path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}


bool ThemeLoader::setTheme(const QString &name)
{
	QString defaultStyleSheet;

	#ifdef Q_OS_WIN
		if (m_settings->value("Interface/scaleFontSize", true).toBool()) {
			// Use a minimum font-size of 9pt for all elements on scaled screens
			double screenRatio = (qApp->primaryScreen()->logicalDotsPerInchX() * qApp->devicePixelRatio()) / 96.0;
			if (screenRatio > 1.0) {
				defaultStyleSheet = "* { font-size: 9pt; }";
			}
		}
	#endif

	const QString dir = QString(m_path).replace('\\', '/') + name + "/";
	const QString cssFile = dir + "style.css";

	QFile f(cssFile);
	if (!f.open(QFile::ReadOnly | QFile::Text)) {
		qApp->setStyleSheet(defaultStyleSheet);
		return false;
	}

	QString css = f.readAll();
	f.close();

	// Replace urls relative paths by absolute ones
	css.replace("url(", "url(" + dir);

	// Update watcher if necessary
	if (m_currentTheme != name) {
		m_currentTheme = name;

		if (!m_watcher.files().isEmpty()) {
			m_watcher.removePaths(m_watcher.files());
		}
		m_watcher.addPath(cssFile);
	}

	qApp->setStyleSheet(defaultStyleSheet + css);
	return true;
}

void ThemeLoader::themeFileChanged()
{
	setTheme(m_currentTheme);
}
