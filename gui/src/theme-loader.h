#ifndef THEME_LOADER_H
#define THEME_LOADER_H

#include <QString>
#include <QSettings>


class ThemeLoader
{
	public:
		explicit ThemeLoader(QString path);
		QStringList getAllThemes() const;
		bool setTheme(QString name);

	private:
		QString m_path;
};

#endif // THEME_LOADER_H
