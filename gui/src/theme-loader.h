#ifndef THEME_LOADER_H
#define THEME_LOADER_H

#include <QSettings>
#include <QString>


class ThemeLoader
{
	public:
		explicit ThemeLoader(const QString &path);
		QStringList getAllThemes() const;
		bool setTheme(const QString &name);

	private:
		QString m_path;
};

#endif // THEME_LOADER_H
