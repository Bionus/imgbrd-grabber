#ifndef THEME_LOADER_H
#define THEME_LOADER_H

#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QStringList>


class QSettings;

class ThemeLoader : public QObject
{
	Q_OBJECT

	public:
		explicit ThemeLoader(QString path, QSettings *settings, QObject *parent = nullptr);
		QStringList getAllThemes() const;
		bool setTheme(const QString &name);

	protected slots:
		void themeFileChanged();

	private:
		QString m_path;
		QSettings *m_settings;
		QString m_currentTheme;
		QFileSystemWatcher m_watcher;
};

#endif // THEME_LOADER_H
