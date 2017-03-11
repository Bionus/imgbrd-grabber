#include "theme-loader.h"
#include <QDir>
#include <QApplication>


ThemeLoader::ThemeLoader(QString path)
	: m_path(path)
{}

QStringList ThemeLoader::getAllThemes() const
{
	return QDir(m_path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}


bool ThemeLoader::setTheme(QString name)
{
	QFile f(m_path + "/" + name + "/style.css");
	if (!f.open(QFile::ReadOnly | QFile::Text))
		return false;

	QString css = f.readAll();
	f.close();
	qApp->setStyleSheet(css);
	return true;
}
