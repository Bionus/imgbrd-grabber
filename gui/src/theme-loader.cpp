#include "theme-loader.h"
#include <QApplication>
#include <QDir>


ThemeLoader::ThemeLoader(const QString &path)
	: m_path(path)
{}

QStringList ThemeLoader::getAllThemes() const
{
	return QDir(m_path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}


bool ThemeLoader::setTheme(const QString &name)
{
	QString dir = QString(m_path).replace('\\', '/') + name + "/";

	QFile f(dir + "style.css");
	if (!f.open(QFile::ReadOnly | QFile::Text))
		return false;

	QString css = f.readAll();
	f.close();

	// Replace urls relative paths by absolute ones
	css.replace("url(", "url(" + dir);

	qApp->setStyleSheet(css);
	return true;
}
