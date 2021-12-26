#ifndef UPDATE_CHECKER_H
#define UPDATE_CHECKER_H

#include <QObject>
#include <QUrl>
#include "updater/program-updater.h"


class QSettings;

class UpdateChecker : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString latestVersion READ latestVersion NOTIFY latestVersionChanged)
	Q_PROPERTY(bool updateAvailable READ updateAvailable NOTIFY updateAvailableChanged)
	Q_PROPERTY(QString changelog READ changelog NOTIFY changelogChanged)
	Q_PROPERTY(QUrl latestUrl READ latestUrl NOTIFY latestUrlChanged)

	public:
		explicit UpdateChecker(QSettings *settings, QObject *parent = nullptr);

		Q_INVOKABLE void checkForUpdates();

		QString latestVersion() const { return m_latestVersion; }
		bool updateAvailable() const { return m_updateAvailable; }
		QString changelog() const { return m_changelog; }
		QUrl latestUrl() const { return m_updater.latestUrl(); }

	protected slots:
		void checkFinished(const QString &latestVersion, bool updateAvailable, const QString &changelog);

	signals:
		void latestVersionChanged();
		void updateAvailableChanged();
		void changelogChanged();
		void latestUrlChanged();

	private:
		ProgramUpdater m_updater;

		QString m_latestVersion = "";
		bool m_updateAvailable = false;
		QString m_changelog = "";
};

#endif // UPDATE_CHECKER_H
