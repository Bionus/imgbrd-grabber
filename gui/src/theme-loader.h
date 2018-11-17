#ifndef THEME_LOADER_H
#define THEME_LOADER_H

#include <QString>
#include <QStringList>


class ThemeLoader
{
	public:
		explicit ThemeLoader(QString path);
		QStringList getAllThemes() const;
		bool setTheme(const QString &name);

	private:
		QString m_path;
};

#endif // THEME_LOADER_H
