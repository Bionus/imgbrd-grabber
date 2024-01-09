#include "updater/program-updater.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <utility>
#include "logger.h"
#include "network/network-manager.h"
#include "network/network-reply.h"


ProgramUpdater::ProgramUpdater()
	: ProgramUpdater(QStringLiteral("https://api.github.com/repos/Bionus/imgbrd-grabber"))
{}

ProgramUpdater::ProgramUpdater(QString baseUrl)
	: m_baseUrl(std::move(baseUrl)), m_downloadReply(nullptr)
{}

void ProgramUpdater::checkForUpdates() const
{
	#ifdef NIGHTLY
		const QUrl url(m_baseUrl + "/releases/tags/nightly");
	#else
		const QUrl url(m_baseUrl + "/releases/latest");
	#endif
	const QNetworkRequest request(url);

	auto *reply = m_networkAccessManager->get(request);
	connect(reply, &NetworkReply::finished, this, &ProgramUpdater::checkForUpdatesDone);
}

void ProgramUpdater::checkForUpdatesDone()
{
	auto *reply = dynamic_cast<NetworkReply*>(sender());
	m_source = reply->readAll();

	QJsonDocument json = QJsonDocument::fromJson(m_source);
	QJsonObject lastRelease = json.object();

	#if defined NIGHTLY
		static const QRegularExpression regexHead(R"(Head:\**\s*([a-f0-9]{40}))");
		const auto match = regexHead.match(lastRelease["body"].toString());
		QString latest = match.hasMatch() ? match.captured(1) : lastRelease["target_commitish"].toString();
		const QString current = QString(NIGHTLY_COMMIT);
		const bool isNew = !current.isEmpty() && latest != current;
		latest = latest.left(8);
		QString changelog;
	#else
		const QString latest = lastRelease["name"].toString().mid(1);
		const bool isNew = compareVersions(latest, QString(VERSION)) > 0;
		const QString changelog = lastRelease["body"].toString();
	#endif

	m_newVersion = latest;
	emit finished(latest, isNew, changelog);
	reply->deleteLater();
}


QUrl ProgramUpdater::latestUrl() const
{
	QJsonDocument json = QJsonDocument::fromJson(m_source);
	QJsonObject lastRelease = json.object();
	return QUrl(lastRelease["html_url"].toString());
}

void ProgramUpdater::downloadUpdate()
{
	QJsonDocument json = QJsonDocument::fromJson(m_source);
	QJsonObject lastRelease = json.object();
	QJsonArray lastAssets = lastRelease["assets"].toArray();
	QJsonObject lastAsset;
	for (int i = 0; i < lastAssets.size(); ++i) {
		const QJsonObject obj = lastAssets[i].toObject();
		const QString name = obj["name"].toString();
		if (name.endsWith(".exe") && name.contains(VERSION_PLATFORM)) {
			lastAsset = obj;
		}
	}
	if (lastAsset.isEmpty()) {
		log("No proper release asset found for updatind", Logger::Error);
		return;
	}

	QUrl url(lastAsset["browser_download_url"].toString());
	m_updateFilename = url.fileName();
	const QNetworkRequest request(url);
	log(QStringLiteral("Downloading installer from \"%1\".").arg(url.toString()));

	m_downloadReply = m_networkAccessManager->get(request);
	connect(m_downloadReply, &NetworkReply::downloadProgress, this, &ProgramUpdater::downloadProgress);
	connect(m_downloadReply, &NetworkReply::finished, this, &ProgramUpdater::downloadDone);
}

void ProgramUpdater::downloadDone()
{
	QUrl redirection = m_downloadReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirection.isEmpty()) {
		log(QStringLiteral("Installer download redirected to \"%1\".").arg(redirection.toString()));
		const QNetworkRequest request(redirection);
		m_downloadReply = m_networkAccessManager->get(request);
		connect(m_downloadReply, &NetworkReply::downloadProgress, this, &ProgramUpdater::downloadProgress);
		connect(m_downloadReply, &NetworkReply::finished, this, &ProgramUpdater::downloadDone);
		return;
	}

	QFile file(QDir::tempPath() + QDir::separator() + m_updateFilename);
	if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
		log(QStringLiteral("Error opening installer file \"%1\".").arg(file.fileName()));
		return;
	}

	file.write(m_downloadReply->readAll());
	file.close();
	log(QStringLiteral("Installer file written to \"%1\"").arg(file.fileName()));

	emit downloadFinished(file.fileName());
}
