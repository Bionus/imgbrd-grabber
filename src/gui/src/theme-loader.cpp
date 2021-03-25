#include "theme-loader.h"
#include <QApplication>
#include <QDir>
#include <QFile>


ThemeLoader::ThemeLoader(QString path, QObject *parent)
	: QObject(parent), m_path(std::move(path))
{
	connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &ThemeLoader::themeFileChanged);
}

QStringList ThemeLoader::getAllThemes() const
{
	return QDir(m_path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}


bool ThemeLoader::setTheme(const QString &name)
{
	const QString dir = QString(m_path).replace('\\', '/') + name + "/";
	const QString cssFile = dir + "style.css";

	QFile f(cssFile);
	if (!f.open(QFile::ReadOnly | QFile::Text)) {
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

	qApp->setStyleSheet(css);
	return true;
}

void ThemeLoader::themeFileChanged()
{
	setTheme(m_currentTheme);
}
