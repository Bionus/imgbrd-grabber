#include "downloader/download-query-manager.h"
#include <QTimer>
#include <utility>
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"
#include "downloader/download-query-loader.h"


DownloadQueryManager::DownloadQueryManager(QString file, Profile *profile)
	: m_file(std::move(file)), m_profile(profile)
{
	m_saveTimer = new QTimer(this);
	m_saveTimer->setInterval(100);
	m_saveTimer->setSingleShot(true);
	connect(m_saveTimer, SIGNAL(timeout()), this, SLOT(save()));
}

bool DownloadQueryManager::load()
{
	return load(m_file);
}
bool DownloadQueryManager::load(const QString &file)
{
	return DownloadQueryLoader::load(file, m_images, m_groups, m_profile);
}

bool DownloadQueryManager::save() const
{
	return save(m_file);
}
bool DownloadQueryManager::save(const QString &file) const
{
	return DownloadQueryLoader::save(file, m_images, m_groups);
}

void DownloadQueryManager::saveLater() const
{
	m_saveTimer->start();
}

void DownloadQueryManager::add(const DownloadQueryImage &image)
{
	m_images.append(image);

	saveLater();
	emit changed();
}
void DownloadQueryManager::add(const DownloadQueryGroup &group)
{
	m_groups.append(group);

	saveLater();
	emit changed();
}

void DownloadQueryManager::remove(const DownloadQueryImage &image)
{
	m_images.removeAll(image);

	saveLater();
	emit changed();
}
void DownloadQueryManager::remove(const DownloadQueryGroup &group)
{
	m_groups.removeAll(group);

	saveLater();
	emit changed();
}

const QList<DownloadQueryImage> &DownloadQueryManager::images() const
{
	return m_images;
}

const QList<DownloadQueryGroup> &DownloadQueryManager::groups() const
{
	return m_groups;
}
