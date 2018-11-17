#include "loader/downloadable-downloader.h"
#include "functions.h"
#include "logger.h"
#include "models/site.h"


DownloadableDownloader::DownloadableDownloader(QSharedPointer<Downloadable> downloadable, Site *site, int count, bool addMd5, bool startCommands, bool loadTags, QObject *parent)
	: QObject(parent), m_downloadable(std::move(downloadable)), m_site(site), m_count(count), m_addMd5(addMd5), m_startCommands(startCommands), m_loadTags(loadTags), m_fileDownloader(false, this)
{}

void DownloadableDownloader::setPath(const Filename &filename, const QString &folder)
{
	m_filename = filename;
	m_folder = folder;
}

void DownloadableDownloader::setPath(const QStringList &paths)
{
	m_paths = paths;
}

void DownloadableDownloader::setResult(const QStringList &keys, Downloadable::SaveResult value)
{
	for (const QString &key : keys)
		m_result.insert(key, value);
}

void DownloadableDownloader::save()
{
	m_downloadable->preload(m_filename);
	preloaded();
}

void DownloadableDownloader::preloaded()
{
	const QUrl url = m_downloadable->url(Downloadable::Size::Full);
	QStringList paths = !m_paths.isEmpty() ? m_paths : m_downloadable->paths(m_filename, m_folder, m_count);

	// Sometimes we don't even need to download the image to save it
	m_paths.clear();
	m_result.clear();
	for (const QString &path : paths)
	{
		Downloadable::SaveResult result = m_downloadable->preSave(path);
		if (result == Downloadable::SaveResult::NotLoaded)
		{ m_paths.append(path); }
		else
		{ m_result.insert(path, result); };
	}

	// If we don't need any loading, we return early
	if (m_paths.isEmpty())
	{
		for (auto it = m_result.constBegin(); it != m_result.constEnd(); ++it)
		{ m_downloadable->postSave(it.key(), it.value(), m_addMd5, m_startCommands, m_count); }
		emit saved(m_downloadable, m_result);
		return;
	}

	// Load the image directly on the disk
	log(QStringLiteral("Loading and saving image in `%1`").arg(m_paths.first()));
	m_url = m_site->fixUrl(url.toString());
	QNetworkReply *reply = m_site->get(m_url, nullptr, QStringLiteral("image"), nullptr); // TODO(Bionus)
	connect(&m_fileDownloader, &FileDownloader::writeError, this, &DownloadableDownloader::writeError, Qt::UniqueConnection);
	connect(&m_fileDownloader, &FileDownloader::networkError, this, &DownloadableDownloader::networkError, Qt::UniqueConnection);
	connect(&m_fileDownloader, &FileDownloader::success, this, &DownloadableDownloader::success, Qt::UniqueConnection);

	// If we can't start writing for some reason, return an error
	if (!m_fileDownloader.start(reply, m_paths))
	{
		log(QStringLiteral("Unable to open file"), Logger::Error);
		setResult(m_paths, Downloadable::SaveResult::Error);
		emit saved(m_downloadable, m_result);
	}
}

void DownloadableDownloader::writeError()
{
	setResult(m_paths, Downloadable::SaveResult::Error);
	emit saved(m_downloadable, m_result);
}

void DownloadableDownloader::networkError(QNetworkReply::NetworkError error, const QString &errorString)
{
	// Ignore cancel errors
	if (error == QNetworkReply::OperationCanceledError)
		return;

	if (error == QNetworkReply::ContentNotFoundError)
	{
		setResult(m_paths, Downloadable::SaveResult::NotFound);
	}
	else
	{
		log(QStringLiteral("Network error for the image: `%1`: %2 (%3)").arg(m_url.toString().toHtmlEscaped()).arg(error).arg(errorString), Logger::Error);
		setResult(m_paths, Downloadable::SaveResult::NetworkError);
	}

	emit saved(m_downloadable, m_result);
}

void DownloadableDownloader::success()
{
	setResult(m_paths, Downloadable::SaveResult::Saved);
	for (const QString &path : qAsConst(m_paths))
	{ m_downloadable->postSave(path, Downloadable::SaveResult::Saved, m_addMd5, m_startCommands, m_count); }
	emit saved(m_downloadable, m_result);
}
