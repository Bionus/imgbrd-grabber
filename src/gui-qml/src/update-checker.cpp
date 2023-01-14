#include "update-checker.h"
#include <QDateTime>
#include <QSettings>
#include "functions.h"


UpdateChecker::UpdateChecker(QSettings *settings, QObject *parent)
	: QObject(parent)
{
	connect(&m_updater, &ProgramUpdater::finished, this, &UpdateChecker::checkFinished);

	const int cfuInterval = settings->value("check_for_updates", 24 * 60 * 60).toInt();
	const QDateTime lastCfu = settings->value("last_check_for_updates", QDateTime()).toDateTime();
	if (cfuInterval >= 0 && (!lastCfu.isValid() || lastCfu.addSecs(cfuInterval) <= QDateTime::currentDateTime())) {
		settings->setValue("last_check_for_updates", QDateTime::currentDateTime());
		m_updater.checkForUpdates();
	}
}

void UpdateChecker::checkForUpdates()
{
	m_updater.checkForUpdates();
}

void UpdateChecker::checkFinished(const QString &latestVersion, bool updateAvailable, const QString &changelog)
{
	if (m_latestVersion != latestVersion) {
		m_latestVersion = latestVersion;
		emit latestVersionChanged();
		emit latestUrlChanged();
	}

	if (m_updateAvailable != updateAvailable) {
		m_updateAvailable = updateAvailable;
		emit updateAvailableChanged();
	}

	if (m_changelog != changelog) {
		m_changelog = parseGithubLinks(changelog);
		emit changelogChanged();
	}
}
